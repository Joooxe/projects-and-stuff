//go:build !solution

package testequal

import (
	"fmt"
)

func compareSimple(ok bool, exp, act any) bool {
	if !ok {
		return false
	}
	return exp == act
}

//func compareContainers(ok bool, exp, act any) bool {
//	exp1, _ := exp.([]interface{})
//	act1, _ := act.([]interface{})
//	if !ok {
//		return false
//	}
//	if (exp == nil) != (act == nil) {
//		return false
//	}
//	if len(exp1) != len(act1) {
//		return false
//	}
//	for i := range exp1 {
//		if exp1[i] != act1[i] {
//			return false
//		}
//	}
//	return true
//}

func isEqual(expected, actual any) bool {
	switch exp := expected.(type) {
	case int:
		act, ok := actual.(int)
		return compareSimple(ok, exp, act)
	case int8:
		act, ok := actual.(int8)
		return compareSimple(ok, exp, act)
	case int16:
		act, ok := actual.(int16)
		return compareSimple(ok, exp, act)
	case int32:
		act, ok := actual.(int32)
		return compareSimple(ok, exp, act)
	case int64:
		act, ok := actual.(int64)
		return compareSimple(ok, exp, act)
	case uint8:
		act, ok := actual.(uint8)
		return compareSimple(ok, exp, act)
	case uint16:
		act, ok := actual.(uint16)
		return compareSimple(ok, exp, act)
	case uint32:
		act, ok := actual.(uint32)
		return compareSimple(ok, exp, act)
	case uint64:
		act, ok := actual.(uint64)
		return compareSimple(ok, exp, act)
	case string:
		act, ok := actual.(string)
		return compareSimple(ok, exp, act)

	// Тут обобщить не получится :(
	case []int:
		act, ok := actual.([]int)
		if !ok {
			return false
		}
		if (exp == nil) != (act == nil) {
			return false
		}
		if len(exp) != len(act) {
			return false
		}
		for i := range exp {
			if exp[i] != act[i] {
				return false
			}
		}
		return true

	case []byte:
		act, ok := actual.([]byte)
		if !ok {
			return false
		}
		if (exp == nil) != (act == nil) {
			return false
		}
		if len(exp) != len(act) {
			return false
		}
		for i := range exp {
			if exp[i] != act[i] {
				return false
			}
		}
		return true

	case map[string]string:
		act, ok := actual.(map[string]string)
		if !ok {
			return false
		}
		if (exp == nil) != (act == nil) {
			return false
		}
		if len(exp) != len(act) {
			return false
		}
		for i := range exp {
			if exp[i] != act[i] {
				return false
			}
		}
		return true

	default:
		return false
	}
}

// AssertEqual checks that expected and actual are equal.
//
// Marks caller function as having failed but continues execution.
//
// Returns true iff arguments are equal.
func AssertEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) bool {
	t.Helper()
	if isEqual(expected, actual) {
		return true
	}
	msg := ""
	if len(msgAndArgs) > 0 {
		msg = fmt.Sprintf(msgAndArgs[0].(string), msgAndArgs[1:]...)
	}
	t.Errorf("not equal:\n\texpected: %v\n\tactual  : %v\n\tmessage : %s", expected, actual, msg)
	return false
}

// AssertNotEqual checks that expected and actual are not equal.
//
// Marks caller function as having failed but continues execution.
//
// Returns true iff arguments are not equal.
func AssertNotEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) bool {
	t.Helper()
	if !isEqual(expected, actual) {
		return true
	}
	msg := ""
	if len(msgAndArgs) > 0 {
		msg = fmt.Sprintf(msgAndArgs[0].(string), msgAndArgs[1:]...)
	}
	t.Errorf("equal:\n\texpected: %v\n\tactual  : %v\n\tmessage : %s", expected, actual, msg)
	return false
}

// RequireEqual does the same as AssertEqual but fails caller test immediately.
func RequireEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) {
	t.Helper()
	if isEqual(expected, actual) {
		return
	}
	msg := ""
	if len(msgAndArgs) > 0 {
		msg = fmt.Sprintf(msgAndArgs[0].(string), msgAndArgs[1:]...)
	}
	t.Errorf("not equal:\n\texpected: %v\n\tactual  : %v\n\tmessage : %s", expected, actual, msg)
	t.FailNow()
}

// RequireNotEqual does the same as AssertNotEqual but fails caller test immediately.
func RequireNotEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) {
	t.Helper()
	if !isEqual(expected, actual) {
		return
	}
	msg := ""
	if len(msgAndArgs) > 0 {
		msg = fmt.Sprintf(msgAndArgs[0].(string), msgAndArgs[1:]...)
	}
	t.Errorf("equal:\n\texpected: %v\n\tactual  : %v\n\tmessage : %s", expected, actual, msg)
	t.FailNow()
}
