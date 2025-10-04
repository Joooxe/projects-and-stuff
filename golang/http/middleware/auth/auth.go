//go:build !solution

package auth

import (
	"context"
	"errors"
	"net/http"
	"strings"
)

type ctxToken struct{}
type User struct {
	Name  string
	Email string
}

func ContextUser(ctx context.Context) (*User, bool) {
	user, ok := ctx.Value(ctxToken{}).(*User)
	return user, ok
}

var ErrInvalidToken = errors.New("invalid token")

type TokenChecker interface {
	CheckToken(ctx context.Context, token string) (*User, error)
}

func CheckAuth(checker TokenChecker) func(next http.Handler) http.Handler {
	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			header := r.Header.Get("Authorization")
			if !strings.HasPrefix(header, "Bearer ") {
				w.WriteHeader(http.StatusUnauthorized)
				return
			}

			token := strings.TrimPrefix(header, "Bearer ")

			user, err := checker.CheckToken(r.Context(), token)
			switch {
			case err == nil:
				ctx := context.WithValue(r.Context(), ctxToken{}, user)
				next.ServeHTTP(w, r.WithContext(ctx))
			case errors.Is(err, ErrInvalidToken):
				w.WriteHeader(http.StatusUnauthorized)
			default:
				w.WriteHeader(http.StatusInternalServerError)
			}
		})
	}
}
