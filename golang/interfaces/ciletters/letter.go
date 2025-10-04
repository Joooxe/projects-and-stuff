package ciletters

import (
	_ "embed"
	"strings"
	"text/template"
)

//go:embed letter.txt
var templ string

func MakeLetter(n *Notification) (string, error) {
	funcMap := template.FuncMap{
		"cut8": func(s string) string {
			if len(s) < 8 {
				return s
			}
			return s[:8]
		},
		"indent": func(spaces int, s string) string {
			prefix := strings.Repeat(" ", spaces)
			lines := strings.Split(s, "\n")
			if len(lines) > 10 {
				lines = lines[len(lines)-10:]
			}
			for i, line := range lines {
				lines[i] = prefix + line
			}
			return strings.Join(lines, "\n")
		},
	}

	tpl, err := template.New("letter.txt").Funcs(funcMap).Parse(templ)
	if err != nil {
		return "", err
	}

	var s strings.Builder

	if err := tpl.Execute(&s, n); err != nil {
		return s.String(), err
	}

	return s.String(), nil
}
