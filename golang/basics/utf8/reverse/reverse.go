//go:build !solution

package reverse

import (
	"strings"
	"unicode/utf8"
)

func Reverse(input string) string {
	var s strings.Builder
	s.Grow(len(input))
	for len(input) > 0 {
		symb, size := utf8.DecodeLastRuneInString(input)
		s.WriteRune(symb)
		input = input[:len(input)-size]
	}
	return s.String()
}
