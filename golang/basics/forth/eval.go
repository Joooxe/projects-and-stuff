//go:build !solution

package main

import (
	"errors"
	"fmt"
	"strconv"
	"strings"
)

// User words (aka brand new instructions)
type Instruction func(e *Evaluator) error
type Evaluator struct {
	stack []int
	words map[string][]Instruction
}

// NewEvaluator creates evaluator.
func NewEvaluator() *Evaluator {
	return &Evaluator{
		stack: []int{},
		words: make(map[string][]Instruction),
	}
}

func (e *Evaluator) push(n int) {
	e.stack = append(e.stack, n)
}

func (e *Evaluator) pop() int {
	val := e.stack[len(e.stack)-1]
	e.stack = e.stack[:len(e.stack)-1]
	return val
}

// Process evaluates sequence of words or definition.
//
// Returns resulting stack state and an error.
func (e *Evaluator) Process(row string) ([]int, error) {
	tokens := strings.Split(row, " ")
	// Format: [":" "word" "{definition...}" ";"]
	if tokens[0] == ":" {
		if len(tokens) < 3 {
			return nil, errors.New("wrong definition")
		}

		word := strings.ToLower(tokens[1])
		if _, err := strconv.Atoi(word); err == nil {
			return nil, errors.New("new word shouldn't be a number")
		}

		var end int
		for i := 2; i < len(tokens); i++ {
			if strings.ToLower(tokens[i]) == ";" {
				end = i
				break
			}
		}
		e.words[word] = e.makeDefinition(tokens[2:end])

		return e.stack, nil
	}

	for _, token := range tokens {
		smolToken := strings.ToLower(token)
		if n, err := strconv.Atoi(smolToken); err == nil {
			e.push(n)

		} else if instructions, ok := e.words[smolToken]; ok {
			for _, instruction := range instructions {
				if err := instruction(e); err != nil {
					return nil, err
				}
			}

		} else if instruction, ok := builtins[smolToken]; ok {
			if err := instruction(e); err != nil {
				return nil, err
			}

		} else {
			return nil, fmt.Errorf("what on earth is this word -> %s", token)
		}
	}

	return e.stack, nil
}

func (e *Evaluator) makeDefinition(tokens []string) []Instruction {
	var definition []Instruction
	for _, token := range tokens {
		token = strings.ToLower(token)
		if n, err := strconv.Atoi(token); err == nil {
			instr := []Instruction{func(e *Evaluator) error {
				e.push(n)
				return nil
			}}
			definition = append(definition, instr...)
		}

		if def, ok := e.words[token]; ok {
			definition = append(definition, def...)
		}

		if op, ok := builtins[token]; ok {
			definition = append(definition, []Instruction{op}...)
		}
	}

	return definition
}

var builtins = map[string]Instruction{
	"+": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use +")
		}
		b := e.pop()
		a := e.pop()
		e.push(a + b)
		return nil
	},
	"-": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use -")
		}
		b := e.pop()
		a := e.pop()
		e.push(a - b)
		return nil
	},
	"*": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use *")
		}
		b := e.pop()
		a := e.pop()
		e.push(a * b)
		return nil
	},
	"/": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use /")
		}
		b := e.pop()
		if b == 0 {
			return errors.New("ayo why are you dividing by zero")
		}
		a := e.pop()
		e.push(a / b)
		return nil
	},
	"dup": func(e *Evaluator) error {
		if len(e.stack) < 1 {
			return errors.New("you have to have at least 1 element in stack to use dup")
		}
		top := e.stack[len(e.stack)-1]
		e.push(top)
		return nil
	},
	"drop": func(e *Evaluator) error {
		if len(e.stack) < 1 {
			return errors.New("you have to have at least 1 element in stack to use drop")
		}
		e.pop()
		return nil
	},
	"swap": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use swap")
		}
		i := len(e.stack) - 1
		e.stack[i], e.stack[i-1] = e.stack[i-1], e.stack[i]
		return nil
	},
	"over": func(e *Evaluator) error {
		if len(e.stack) < 2 {
			return errors.New("you have to have at least 2 elements in stack to use over")
		}
		e.push(e.stack[len(e.stack)-2])
		return nil
	},
}
