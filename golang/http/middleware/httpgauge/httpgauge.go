//go:build !solution

package httpgauge

import (
	"fmt"
	"github.com/go-chi/chi/v5"
	"net/http"
	"sort"
	"sync"
)

type Gauge struct {
	mu      sync.Mutex
	metrics map[string]int
}

func New() *Gauge {
	return &Gauge{
		metrics: make(map[string]int),
	}
}

func (g *Gauge) Snapshot() map[string]int {
	g.mu.Lock()
	defer g.mu.Unlock()

	metricsCopy := make(map[string]int, len(g.metrics))
	for k, v := range g.metrics {
		metricsCopy[k] = v
	}
	return metricsCopy
}

// ServeHTTP returns accumulated statistics in text format ordered by pattern.
//
// For example:
//
//	/a 10
//	/b 5
//	/c/{id} 7
func (g *Gauge) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	metrics := g.Snapshot()
	patterns := make([]string, 0, len(metrics))
	for p := range metrics {
		patterns = append(patterns, p)
	}

	sort.Strings(patterns)
	for _, p := range patterns {
		fmt.Fprintf(w, "%s %d\n", p, metrics[p])
	}
}

func (g *Gauge) Wrap(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		defer func() {
			patern := chi.RouteContext(r.Context()).RoutePattern()

			g.mu.Lock()
			g.metrics[patern]++
			g.mu.Unlock()
		}()

		next.ServeHTTP(w, r)
	})
}
