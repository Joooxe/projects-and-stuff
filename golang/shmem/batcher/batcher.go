//go:build !solution

package batcher

import (
	"sync"
	"time"

	"gitlab.com/slon/shad-go/batcher/slow"
)

const delta = time.Millisecond

type Batcher struct {
	v *slow.Value

	mu sync.Mutex

	active    bool
	startTime time.Time
	done      chan struct{}
	value     interface{}
}

func NewBatcher(v *slow.Value) *Batcher {
	return &Batcher{v: v}
}

func (b *Batcher) Load() interface{} {
	call := time.Now()

	b.mu.Lock()
	if b.active {
		// we in this batch, wait till result
		if call.Sub(b.startTime) < delta {
			done := b.done
			b.mu.Unlock()
			<-done
			return b.value
		}
		// else wait till result and retry, we live in the next batch
		done := b.done
		b.mu.Unlock()
		<-done
		return b.Load()
	}
	b.active = true
	b.startTime = time.Now()
	b.done = make(chan struct{})
	b.mu.Unlock()

	// gathering loads
	time.Sleep(delta)

	result := b.v.Load()

	b.mu.Lock()
	b.value = result
	b.active = false
	done := b.done
	b.mu.Unlock()

	close(done)
	return result
}
