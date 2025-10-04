//go:build !solution

package hotelbusiness

import (
	"sort"
)

type Guest struct {
	CheckInDate  int
	CheckOutDate int
}

type Load struct {
	StartDate  int
	GuestCount int
}

type Event struct{ day, diff int }

func ComputeLoad(guests []Guest) []Load {
	events := make([]Event, 0, len(guests)*2)
	for _, guest := range guests {
		events = append(events, Event{day: guest.CheckInDate, diff: 1})
		events = append(events, Event{day: guest.CheckOutDate, diff: -1})
	}

	sort.Slice(events, func(i, j int) bool {
		if events[i].day != events[j].day {
			return events[i].day < events[j].day
		}
		return events[i].diff < events[j].diff
	})

	load := make([]Load, 0, len(events))
	cnt := 0
	oldCnt := 0
	for i := 0; i < len(events); {
		day := events[i].day
		for i < len(events) && day == events[i].day {
			cnt += events[i].diff
			i++
		}
		if cnt == oldCnt {
			continue
		}
		load = append(load, Load{StartDate: day, GuestCount: cnt})
		oldCnt = cnt
	}
	return load
}
