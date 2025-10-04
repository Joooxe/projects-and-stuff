//go:build !solution

package main

import (
	"encoding/json"
	"math/rand"
	"net/http"
	"os"
	"sync"
	"time"
)

var (
	urlToKey = make(map[string]string)
	keyToURL = make(map[string]string)
	mu       sync.Mutex
	rnd      *rand.Rand
	req      struct {
		URL string `json:"url"`
	}
)

const letters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

func generateRandomKey(n int) string {
	b := make([]byte, n)
	for i := range b {
		b[i] = letters[rnd.Intn(len(letters))]
	}
	return string(b)
}

func collisionCheck(keyToURL map[string]string) (key string) {
	for {
		key = generateRandomKey(6)
		if _, used := keyToURL[key]; !used {
			break
		}
	}
	return key
}

func shortenHandler(w http.ResponseWriter, r *http.Request) {
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		w.Header().Set("X-Content-Type-Options", "nosniff")
		w.WriteHeader(http.StatusBadRequest) // 400
		return
	}

	mu.Lock()
	if key, ok := urlToKey[req.URL]; ok {
		mu.Unlock()
		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]string{ // 200
			"url": req.URL,
			"key": key,
		})
		return
	}

	key := collisionCheck(keyToURL)
	urlToKey[req.URL] = key
	keyToURL[key] = req.URL
	mu.Unlock()

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{ // 200
		"url": req.URL,
		"key": key,
	})
}

func goHandler(w http.ResponseWriter, r *http.Request) {
	key := r.URL.Path[4:] // "/go/"
	mu.Lock()
	target, ok := keyToURL[key]
	mu.Unlock()
	if !ok {
		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		w.Header().Set("X-Content-Type-Options", "nosniff")
		w.WriteHeader(http.StatusNotFound) // 404
		return
	}

	w.Header().Set("Location", target)
	w.WriteHeader(http.StatusFound) // 302
}

func main() {
	rnd = rand.New(rand.NewSource(time.Now().UnixNano()))

	port := os.Args[2]
	addr := ":" + port

	mux := http.NewServeMux()
	mux.HandleFunc("/shorten", shortenHandler)
	mux.HandleFunc("/go/", goHandler)

	if err := http.ListenAndServe(addr, mux); err != nil {
		panic(err)
	}
}
