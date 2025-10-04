package varfmt

import (
	"fmt"
	"strconv"
	"strings"
)

func Sprintf(format string, args ...interface{}) string {
	cacheStrArgs := make([]string, len(args))
	for i, arg := range args {
		cacheStrArgs[i] = fmt.Sprint(arg)
	}

	var b strings.Builder
	b.Grow(len(format))
	cnt := 0
	i := 0

	for i < len(format) {
		if format[i] == '{' {
			st := i + 1
			end := strings.IndexByte(format[st:], '}') + st
			strArgInd := format[st:end]
			var argInd int

			if strArgInd == "" {
				argInd = cnt
			} else {
				n, err := strconv.Atoi(strArgInd)
				if err != nil {
					panic("invalid strArgInd")
				}
				argInd = n
			}
			b.WriteString(cacheStrArgs[argInd])

			i = end + 1
			cnt++
		} else {
			b.WriteByte(format[i])
			i++
		}
	}
	return b.String()
}
