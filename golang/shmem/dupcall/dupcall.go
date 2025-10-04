//go:build !solution

package dupcall

import (
	"context"
	"sync"
)

type Call struct {
	mu         sync.Mutex
	done       chan struct{}
	running    bool
	res        interface{}
	err        error
	cntWaiters int
	cancelFunc context.CancelFunc
}

func (o *Call) Do(
	ctx context.Context,
	cb func(context.Context) (interface{}, error),
) (result interface{}, err error) {
	o.mu.Lock()

	if o.running {
		o.cntWaiters++
		o.mu.Unlock()

		select {
		case <-ctx.Done():
			o.mu.Lock()
			o.cntWaiters--

			if o.cntWaiters == 0 && o.cancelFunc != nil {
				o.cancelFunc()
			}
			o.mu.Unlock()
			return nil, ctx.Err()

		case <-o.done:
			o.mu.Lock()
			res, err := o.res, o.err
			o.mu.Unlock()
			return res, err
		}
	}

	innerCtx, cancel := context.WithCancel(context.Background())
	o.running = true
	o.cntWaiters = 1
	o.done = make(chan struct{})
	o.cancelFunc = cancel
	o.mu.Unlock()

	go func() {
		res, err := cb(innerCtx)
		o.mu.Lock()

		o.res = res
		o.err = err
		close(o.done)
		o.running = false

		o.mu.Unlock()
	}()

	select {
	case <-ctx.Done():
		o.mu.Lock()
		o.cntWaiters--

		if o.cntWaiters == 0 && o.cancelFunc != nil {
			o.cancelFunc()
		}
		o.mu.Unlock()
		return nil, ctx.Err()

	case <-o.done:
		o.mu.Lock()
		res, err := o.res, o.err
		o.mu.Unlock()
		return res, err
	}
}
