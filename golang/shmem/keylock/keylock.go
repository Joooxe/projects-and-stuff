//go:build !solution

package keylock

import (
	"sync"
)

type KeyLock struct {
	mu     sync.Mutex
	locked map[string]bool
	notify chan struct{}
}

func New() *KeyLock {
	return &KeyLock{
		locked: make(map[string]bool),
		notify: make(chan struct{}, 1),
	}
}

func (l *KeyLock) LockKeys(keys []string, cancel <-chan struct{}) (canceled bool, unlock func()) {
	for {

		l.mu.Lock()
		canLock := true
		for _, key := range keys {
			if l.locked[key] {
				canLock = false
				break
			}
		}

		if canLock {
			for _, key := range keys {
				l.locked[key] = true
			}
			l.mu.Unlock()

			unlock = func() {
				l.mu.Lock()
				for _, key := range keys {
					delete(l.locked, key)
				}
				select {
				case l.notify <- struct{}{}:
				default:
				}
				l.mu.Unlock()
			}

			return false, unlock
		}
		l.mu.Unlock()

		select {
		case <-cancel:
			return true, nil
		case <-l.notify:
			// tries to get previously blocked key again
		}
	}
}
