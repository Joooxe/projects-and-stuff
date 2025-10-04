//go:build !solution

package requestlog

import (
	"net/http"
	"strconv"
	"sync/atomic"
	"time"

	"go.uber.org/zap"
)

type rw struct {
	http.ResponseWriter
	status int
}

func (w *rw) WriteHeader(status int) {
	w.status = status
	w.ResponseWriter.WriteHeader(status)
}

func Log(l *zap.Logger) func(next http.Handler) http.Handler {
	var cnt uint64

	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			reqID := strconv.FormatUint(atomic.AddUint64(&cnt, 1), 10)

			l.Info("request started",
				zap.String("request_id", reqID),
				zap.String("path", r.URL.Path),
				zap.String("method", r.Method),
			)

			start := time.Now()
			writer := &rw{ResponseWriter: w, status: http.StatusOK}

			defer func() {
				duration := time.Since(start)
				if rec := recover(); rec != nil {
					l.Info("request panicked",
						zap.String("request_id", reqID),
						zap.String("path", r.URL.Path),
						zap.String("method", r.Method),
						zap.Duration("duration", duration),
						zap.Int("status_code", writer.status),
					)
					panic(rec)
				}

				l.Info("request finished",
					zap.String("request_id", reqID),
					zap.String("path", r.URL.Path),
					zap.String("method", r.Method),
					zap.Duration("duration", duration),
					zap.Int("status_code", writer.status),
				)
			}()

			next.ServeHTTP(writer, r)
		})
	}
}
