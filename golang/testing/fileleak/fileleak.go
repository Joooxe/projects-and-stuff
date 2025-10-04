//go:build !solution

package fileleak

import (
	"os"
)

type testingT interface {
	Errorf(msg string, args ...interface{})
	Cleanup(func())
}

func getFD() map[string]int {
	fds := make(map[string]int)
	entries, _ := os.ReadDir("/proc/self/fd")
	for _, entry := range entries {
		path := "/proc/self/fd/" + entry.Name()
		target, _ := os.Readlink(path)
		fds[target]++
	}
	return fds
}

func VerifyNone(t testingT) {
	initial := getFD()
	t.Cleanup(func() {
		final := getFD()
		for target, finalCount := range final {
			if initCount := initial[target]; finalCount > initCount {
				t.Errorf("file leak detected:\n")
				break
			}
		}
	})
}
