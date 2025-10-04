//go:build !solution

package cond

type token struct{}

// A Locker represents an object that can be locked and unlocked.
type Locker interface {
	Lock()
	Unlock()
}

// Cond implements a condition variable, a rendezvous point
// for goroutines waiting for or announcing the occurrence
// of an event.
//
// Each Cond has an associated Locker L (often a *sync.Mutex or *sync.RWMutex),
// which must be held when changing the condition and
// when calling the Wait method.
type Cond struct {
	L         Locker
	waits     []chan token
	innerLock chan token
}

// New returns a new Cond with Locker l.
func New(l Locker) *Cond {
	c := &Cond{
		L:         l,
		waits:     make([]chan token, 0),
		innerLock: make(chan token, 1),
	}
	c.innerLock <- token{}
	return c
}

// Wait atomically unlocks c.L and suspends execution
// of the calling goroutine. After later resuming execution,
// Wait locks c.L before returning. Unlike in other systems,
// Wait cannot return unless awoken by Broadcast or Signal.
//
// Because c.L is not locked when Wait first resumes, the caller
// typically cannot assume that the condition is true when
// Wait returns. Instead, the caller should Wait in a loop:
//
//	c.L.Lock()
//	for !condition() {
//	    c.Wait()
//	}
//	... make use of condition ...
//	c.L.Unlock()
func (c *Cond) Wait() {
	ch := make(chan token)

	<-c.innerLock
	c.waits = append(c.waits, ch)
	c.innerLock <- token{}

	c.L.Unlock()
	<-ch
	c.L.Lock()
}

// Signal wakes one goroutine waiting on c, if there is any.
//
// It is allowed but not required for the caller to hold c.L
// during the call.
func (c *Cond) Signal() {
	<-c.innerLock

	if len(c.waits) > 0 {
		defer close(c.waits[0])
		c.waits = c.waits[1:]
	}

	c.innerLock <- token{}
}

// Broadcast wakes all goroutines waiting on c.
//
// It is allowed but not required for the caller to hold c.L
// during the call.
func (c *Cond) Broadcast() {
	<-c.innerLock

	for _, ch := range c.waits {
		close(ch)
	}
	c.waits = c.waits[0:0]

	c.innerLock <- token{}
}
