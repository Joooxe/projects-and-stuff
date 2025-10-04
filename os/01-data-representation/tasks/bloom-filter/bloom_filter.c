#include <stdlib.h>
#include <stddef.h>

#include "bloom_filter.h"

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
    uint64_t hash = 0;
    for (; *str; ++str) {
        hash = (hash * seed + *str) % modulus;
    }
    return hash;
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count) {
    bloom_filter->set_size = set_size;
    if (set_size % 64 == 0) {
        bloom_filter->set = calloc(set_size / 64, sizeof(uint64_t));
    } else {
        bloom_filter->set = calloc(set_size / 64 + 1, sizeof(uint64_t));
    }
    bloom_filter->hash_fn = hash_fn;
    bloom_filter->hash_fn_count = hash_fn_count;
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
    free(bloom_filter->set);
    bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
    for (uint64_t i = 1; i < bloom_filter->hash_fn_count + 1; i++) {
        uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
        uint64_t bit_pos = hash % bloom_filter->set_size;
        bloom_filter->set[bit_pos / 64] |= (uint64_t)1 << (bit_pos % 64);
    }
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
    for (uint64_t i = 1; i < bloom_filter->hash_fn_count + 1; i++) {
        uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
        uint64_t bit_pos = hash % bloom_filter->set_size;
        if (!(bloom_filter->set[bit_pos / 64] & (uint64_t)1 << (bit_pos % 64))) {
            return false;
        }
    }
    return true;
}
