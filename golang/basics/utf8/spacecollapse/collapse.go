//go:build !solution

package spacecollapse

import (
	"strings"
	"unicode"
	"unicode/utf8"
)

func CollapseSpaces(input string) string {
	var s strings.Builder
	s.Grow(len(input))
	isPrevNotSpace := true

	for len(input) > 0 {
		r, size := utf8.DecodeRuneInString(input)
		input = input[size:]
		if unicode.IsSpace(r) { //check "пробельные символы"
			if isPrevNotSpace {
				s.WriteRune(' ') //write "символ "пробел""
				isPrevNotSpace = false
			}
		} else {
			s.WriteRune(r)
			isPrevNotSpace = true
		}
	}

	return s.String()
}
