//go:build !solution

package pubsub

import (
	"context"
	"fmt"
	"sync"
)

var _ Subscription = (*MySubscription)(nil)

type MySubscription struct {
	subj   string
	cb     MsgHandler
	parent *MyPubSub

	mu      sync.Mutex
	cond    *sync.Cond
	msgs    []interface{}
	isUnsub bool
}

func (s *MySubscription) Unsubscribe() {
	s.parent.mu.Lock()
	if subsMap, ok := s.parent.subs[s.subj]; ok {
		delete(subsMap, s)
	}
	s.parent.mu.Unlock()

	s.mu.Lock()
	s.isUnsub = true
	s.msgs = nil
	s.cond.Signal()
	s.mu.Unlock()

}

var _ PubSub = (*MyPubSub)(nil)

type MyPubSub struct {
	mu     sync.Mutex
	subs   map[string]map[*MySubscription]struct{}
	finita bool
	wg     sync.WaitGroup
}

func NewPubSub() PubSub {
	return &MyPubSub{
		subs: make(map[string]map[*MySubscription]struct{}),
	}
}

func (p *MyPubSub) Subscribe(subj string, cb MsgHandler) (Subscription, error) {
	p.mu.Lock()
	defer p.mu.Unlock()

	if p.finita {
		return nil, fmt.Errorf("pubsub closed")
	}

	if p.subs[subj] == nil {
		p.subs[subj] = make(map[*MySubscription]struct{})
	}

	s := &MySubscription{
		subj:   subj,
		cb:     cb,
		parent: p,
	}
	s.cond = sync.NewCond(&s.mu)
	p.subs[subj][s] = struct{}{}

	p.wg.Add(1)
	go func() {
		defer p.wg.Done()
		s.deliverLoop()
	}()
	return s, nil
}

func (p *MyPubSub) Publish(subj string, msg interface{}) error {
	p.mu.Lock()
	if p.finita {
		p.mu.Unlock()
		return fmt.Errorf("pubsub closed")
	}

	subsMap, ok := p.subs[subj]
	if !ok {
		p.mu.Unlock()
		return nil
	}

	subs := make([]*MySubscription, 0, len(subsMap))
	for s := range subsMap {
		subs = append(subs, s)
	}
	p.mu.Unlock()

	for _, s := range subs {
		s.mu.Lock()
		s.msgs = append(s.msgs, msg)
		s.cond.Signal()
		s.mu.Unlock()
	}
	return nil
}

func (p *MyPubSub) Close(ctx context.Context) error {
	p.mu.Lock()
	p.finita = true
	for _, subs := range p.subs {
		for s := range subs {
			s.mu.Lock()
			s.isUnsub = true
			s.cond.Signal()
			s.mu.Unlock()
		}
	}
	p.mu.Unlock()

	done := make(chan struct{})
	go func() {
		p.wg.Wait()
		close(done)
	}()
	select {
	case <-done:
		return nil
	case <-ctx.Done():
		return ctx.Err()
	}
}

func (s *MySubscription) deliverLoop() {
	for {
		s.mu.Lock()
		for len(s.msgs) == 0 && !s.isUnsub {
			s.cond.Wait()
		}

		if len(s.msgs) == 0 && s.isUnsub {
			s.mu.Unlock()
			return
		}
		// FIFO
		msg := s.msgs[0]
		s.msgs = s.msgs[1:]
		s.mu.Unlock()

		s.cb(msg)
	}
}
