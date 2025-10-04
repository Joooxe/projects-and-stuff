//go:build !solution

package hogwarts

import "fmt"

const (
	white = iota
	gray
	black
)

func DFS(course string, prereqs map[string][]string, node map[string]int, result *[]string) {
	if node[course] == gray {
		panic("cycle detected")
	}
	if node[course] == black {
		return
	}
	node[course] = gray
	for _, prereq := range prereqs[course] {
		DFS(prereq, prereqs, node, result)
	}
	node[course] = black
	*result = append(*result, course)
}

func GetCourseList(prereqs map[string][]string) []string {
	node := make(map[string]int)
	result := make([]string, 0)
	for course := range prereqs {
		if node[course] == white {
			DFS(course, prereqs, node, &result)
		}
	}
	fmt.Print(result)
	return result
}
