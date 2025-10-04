//go:build !solution

package lrucache

import (
	"container/list"
)

type entry struct {
	key   int
	value int
}

type lruCache struct {
	capacity int
	items    map[int]*list.Element
	list     *list.List
}

func New(cap int) Cache {
	return &lruCache{
		capacity: cap,
		items:    make(map[int]*list.Element),
		list:     list.New(),
	}
}

func (lc *lruCache) Get(key int) (int, bool) {
	le, ok := lc.items[key]
	if !ok {
		return 0, false
	}

	lc.list.MoveToBack(le)
	ent := le.Value.(*entry)
	return ent.value, true
}

func (lc *lruCache) Set(key, value int) {
	if lc.capacity == 0 {
		return
	}

	if le, ok := lc.items[key]; ok {
		ent := le.Value.(*entry)
		ent.value = value
		lc.list.MoveToBack(le)
		return
	}

	if lc.list.Len() >= lc.capacity {
		front := lc.list.Front()
		ent := front.Value.(*entry)
		delete(lc.items, ent.key)
		lc.list.Remove(front)
	}
	ent := &(entry{key: key, value: value})
	le := lc.list.PushBack(ent)
	lc.items[key] = le
}

func (lc *lruCache) Range(f func(key, value int) bool) {
	for e := lc.list.Front(); e != nil; e = e.Next() {
		ent := e.Value.(*entry)
		if !f(ent.key, ent.value) {
			break
		}
	}
}

func (lc *lruCache) Clear() {
	lc.items = make(map[int]*list.Element)
	lc.list.Init()
}
