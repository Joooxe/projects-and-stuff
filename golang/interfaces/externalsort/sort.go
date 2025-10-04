//go:build !solution

package externalsort

import (
	"container/heap"
	"io"
	"os"
	"sort"
)

type lineReader struct {
	r io.Reader
}

func NewReader(r io.Reader) LineReader {
	return &lineReader{r: r}
}

type lineWriter struct {
	w io.Writer
}

func NewWriter(w io.Writer) LineWriter {
	return &lineWriter{w: w}
}

func (lr *lineReader) ReadLine() (string, error) {
	var line []byte
	buf := make([]byte, 1)
	for {
		n, err := lr.r.Read(buf)
		if n > 0 {
			if buf[0] == '\n' {
				return string(line), err
			}
			line = append(line, buf[0])
		}
		if err != nil {
			if len(line) != 0 {
				return string(line), err
			}
			return "", err
		}
	}
}

func (lw *lineWriter) Write(l string) error {
	_, err := lw.w.Write([]byte(l))
	if err != nil {
		return err
	}

	_, err = lw.w.Write([]byte("\n"))
	return err
}

type heapItem struct {
	value       string
	readerIndex int
}

type minHeap []heapItem

func (h minHeap) Len() int { return len(h) }

func (h minHeap) Less(i, j int) bool { return h[i].value < h[j].value }

func (h minHeap) Swap(i, j int) { h[i], h[j] = h[j], h[i] }

func (h *minHeap) Push(x any) { *h = append(*h, x.(heapItem)) }

func (h *minHeap) Pop() any {
	old := *h
	n := len(old)
	item := old[n-1]
	*h = old[0 : n-1]
	return item
}

func Merge(w LineWriter, readers ...LineReader) error {
	h := &minHeap{}
	for i, r := range readers {
		line, err := r.ReadLine()
		if err != nil {
			if len(line) != 0 {
				heap.Push(h, heapItem{value: line, readerIndex: i})
			}
			continue
		}
		heap.Push(h, heapItem{value: line, readerIndex: i})
	}
	heap.Init(h)

	for h.Len() > 0 {
		item := heap.Pop(h).(heapItem)
		if err := w.Write(item.value); err != nil {
			return err
		}

		line, err := readers[item.readerIndex].ReadLine()
		if err != nil {
			if len(line) != 0 {
				heap.Push(h, heapItem{value: line, readerIndex: item.readerIndex})
			}
			continue
		}
		heap.Push(h, heapItem{value: line, readerIndex: item.readerIndex})
	}
	return nil
}

func Sort(w io.Writer, in ...string) error {
	for _, fileName := range in {
		f, err := os.Open(fileName)
		if err != nil {
			return err
		}
		defer f.Close()

		lr := NewReader(f)
		var lines []string
		for {
			line, readErr := lr.ReadLine()
			if readErr != nil {
				if line != "" {
					lines = append(lines, line)
				}
				break
			}
			lines = append(lines, line)
		}

		sort.Strings(lines)

		fw, err := os.Create(fileName)
		if err != nil {
			return err
		}
		defer fw.Close()

		lw := NewWriter(fw)
		for _, line := range lines {
			if err := lw.Write(line); err != nil {
				return err
			}
		}
	}

	var readers []LineReader
	for _, fileName := range in {
		f, err := os.Open(fileName)
		if err != nil {
			return err
		}
		defer f.Close()

		readers = append(readers, NewReader(f))
	}

	return Merge(NewWriter(w), readers...)
}
