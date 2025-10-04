//go:build !solution

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"net/http"
	"strconv"
	"strings"
	"time"
)

var symbolMap = map[rune]string{
	'0': Zero,
	'1': One,
	'2': Two,
	'3': Three,
	'4': Four,
	'5': Five,
	'6': Six,
	'7': Seven,
	'8': Eight,
	'9': Nine,
	':': Colon,
}

func clockHandler(w http.ResponseWriter, r *http.Request) {
	q := r.URL.Query()

	k, err := strconv.Atoi(q.Get("k"))
	if err != nil || k < 1 || k > 30 {
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	s := q.Get("time")
	t, _ := time.Parse("15:04:05", s)
	timeStr := t.Format("15:04:05")

	rowsPerSymbol := make([][]string, 0, len(timeStr))
	for _, ch := range timeStr {
		s, ok := symbolMap[ch]
		if !ok {
			http.Error(w, "invalid time", http.StatusBadRequest)
			return
		}
		rows := strings.Split(s, "\n")
		rowsPerSymbol = append(rowsPerSymbol, rows)
	}

	symbolH := len(rowsPerSymbol[0])
	baseW := 0
	for _, rows := range rowsPerSymbol {
		baseW += len(rows[0])
	}

	wPx := baseW * k
	hPx := symbolH * k

	img := image.NewRGBA(image.Rect(0, 0, wPx, hPx))
	for y := 0; y < hPx; y++ {
		for x := 0; x < wPx; x++ {
			img.Set(x, y, color.White)
		}
	}

	offsetX := 0
	for _, rows := range rowsPerSymbol {
		symW := len(rows[0])
		for sy, row := range rows {
			for sx, c := range row {
				if c == '1' {
					for dy := 0; dy < k; dy++ {
						for dx := 0; dx < k; dx++ {
							x := offsetX + sx*k + dx
							y := sy*k + dy
							img.Set(x, y, Cyan)
						}
					}
				}
			}
		}
		offsetX += symW * k
	}

	w.Header().Set("Content-Type", "image/png")
	png.Encode(w, img) // 200
}

func main() {
	port := flag.Int("port", 0, "bruh")
	flag.Parse()

	http.HandleFunc("/", clockHandler)
	addr := fmt.Sprintf(":%d", *port)
	if err := http.ListenAndServe(addr, nil); err != nil {
		panic(err)
	}
}
