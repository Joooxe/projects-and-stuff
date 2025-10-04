#include <stdint.h>
#include <string.h>

#include "ieee754_clf.h"

float_class_t classify(double value) {
    uint64_t mem;
    memcpy(&mem, &value, sizeof(mem));

    uint64_t sign = (mem >> 63) & 1;
    uint64_t exponent = (mem >> 52) & 0x7FF;
    uint64_t mantissa = mem & 0xFFFFFFFFFFFFF;

    if (exponent == 0) {
        if (mantissa == 0) {
            return sign ? MinusZero : Zero;
        }
        return sign ? MinusDenormal : Denormal;
    }
    if (exponent == 0x7FF) {
        if (mantissa == 0) {
            return sign ? MinusInf : Inf;
        }
        return NaN;
    }
    return sign ? MinusRegular : Regular;
}
// comment for testing system