//go:build !solution

package reversemap

import "reflect"

func ReverseMap(forward interface{}) interface{} {
	v := reflect.ValueOf(forward)

	keyType := v.Type().Key()
	valType := v.Type().Elem()
	reversedMapType := reflect.MapOf(valType, keyType)

	result := reflect.MakeMapWithSize(reversedMapType, v.Len())
	for _, key := range v.MapKeys() {
		val := v.MapIndex(key)
		result.SetMapIndex(val, key)
	}
	return result.Interface()
}
