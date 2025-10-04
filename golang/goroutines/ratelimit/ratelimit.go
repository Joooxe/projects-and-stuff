//go:build !solution

package ratelimit

import (
	"context"
	"errors"
	"time"
)

type token struct{}

// Limiter is precise rate limiter with context support.
type Limiter struct {
	maxCount int
	interval time.Duration

	callOClock []time.Time

	mu      chan token
	stopCh  chan token
	stopped bool
}

var ErrStopped = errors.New("ayo: ErrStopped")

// NewLimiter returns limiter that throttles rate of successful Acquire() calls
// to maxSize events at any given interval.
func NewLimiter(maxCount int, interval time.Duration) *Limiter {
	l := &Limiter{
		maxCount:   maxCount,
		interval:   interval,
		callOClock: make([]time.Time, 0, maxCount),
		mu:         make(chan token, 1),
		stopCh:     make(chan token, 0),
	}
	l.mu <- token{}
	return l
}

func (l *Limiter) Acquire(ctx context.Context) error {
	for {
		<-l.mu

		if l.stopped {
			l.mu <- token{}
			return ErrStopped
		}

		for len(l.callOClock) > 0 && time.Since(l.callOClock[0]) >= l.interval {
			l.callOClock = l.callOClock[1:]
		}
		if len(l.callOClock) < l.maxCount {
			l.callOClock = append(l.callOClock, time.Now())
			l.mu <- token{}
			return nil
		}

		deadline := l.interval - time.Since(l.callOClock[0])
		l.mu <- token{}

		select {
		case <-time.After(deadline):
			// Enough waining
		case <-ctx.Done():
			return ctx.Err()
		case <-l.stopCh:
			return ErrStopped
		}
	}
}

func (l *Limiter) Stop() {
	<-l.mu
	if !l.stopped {
		l.stopped = true
		close(l.stopCh)
	}
	l.mu <- token{}
}
