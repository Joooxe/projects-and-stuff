//go:build !solution

package speller

import (
	"fmt"
	"strings"
)

var ones = [10]string{
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
}

var teens = [10]string{
	"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen",
}

var tensNames = [10]string{
	"", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety",
}

func SpellUnder100(n int64) string {
	if n < 10 {
		return ones[n]
	} else if n < 20 {
		return teens[n-10]
	} else {
		tens := n / 10
		rem := n % 10
		if rem == 0 {
			return tensNames[tens]
		}
		return fmt.Sprintf("%s-%s", tensNames[tens], ones[rem])
	}
}

func SpellUnder1000(n int64) string {
	parts := []string{}
	if n >= 100 {
		parts = append(parts, ones[n/100])
		parts = append(parts, "hundred")
		n %= 100
	}
	if n > 0 {
		parts = append(parts, SpellUnder100(n))
	}
	return strings.Join(parts, " ")
}

func Spell(n int64) string {
	if n == 0 {
		return "zero"
	}

	result := []string{}
	if n < 0 {
		result = append(result, "minus")
		n = -n
	}

	billions := n / 1_000_000_000
	n %= 1_000_000_000
	millions := n / 1_000_000
	n %= 1_000_000
	thousands := n / 1000
	remain := n % 1000

	if billions != 0 {
		result = append(result, SpellUnder1000(billions))
		result = append(result, "billion")
	}
	if millions != 0 {
		result = append(result, SpellUnder1000(millions))
		result = append(result, "million")
	}
	if thousands != 0 {
		result = append(result, SpellUnder1000(thousands))
		result = append(result, "thousand")
	}
	if remain != 0 {
		result = append(result, SpellUnder1000(remain))
	}

	return strings.Join(result, " ")
}
