//go:build !solution

package otp

import (
	"io"
)

type streamReader struct {
	r    io.Reader
	prng io.Reader
}

func (sr *streamReader) Read(p []byte) (int, error) {
	n, err := sr.r.Read(p)

	for i := 0; i < n; i++ {
		key := make([]byte, 1)
		_, _ = io.ReadFull(sr.prng, key[:])
		p[i] ^= key[0]
	}

	return n, err
}

func NewReader(r io.Reader, prng io.Reader) io.Reader {
	return &streamReader{r: r, prng: prng}
}

type streamWriter struct {
	w    io.Writer
	prng io.Reader
}

func (sw *streamWriter) Write(p []byte) (int, error) {
	buf := make([]byte, len(p))
	copy(buf, p)

	for i := range buf {
		key := make([]byte, 1)
		_, _ = io.ReadFull(sw.prng, key[:])
		buf[i] ^= key[0]
	}

	return sw.w.Write(buf)
}

func NewWriter(w io.Writer, prng io.Reader) io.Writer {
	return &streamWriter{w: w, prng: prng}
}
