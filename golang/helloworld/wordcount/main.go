//go:build !solution

package main

import (
	"fmt"
	"os"
	"strings"
)

func WordCount(s string, m map[string]int) map[string]int {
	arr := strings.Split(s, "\n")
	for _, elem := range arr {
		m[elem] += 1
	}
	return m
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func main() {
	m := make(map[string]int)
	for _, arg := range os.Args[1:] {
		dat, err := os.ReadFile(arg)
		check(err)
		WordCount(string(dat), m)
	}
	for line, count := range m {
		if count >= 2 {
			fmt.Printf("%d\t%s\n", count, line)
		}
	}
}
