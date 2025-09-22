#ifndef _RANDOM_HEADER
#define _RANDOM_HEADER

#include "utils.h"
#include <stddef.h>
#include <stdint.h>

typedef struct RandomState RandomState;

// Creates a new RandomState and seeds it using the current time and a stack
// address for entropy.
RandomState *random_make();

void random_destroy(RandomState *state);

// -------------------- integer methods  -------------------

// Returns a random integer: min <= n < max
uint64_t random_range(RandomState *state, uint64_t min, uint64_t max);

// -------------------- sequence methods  -------------------

// Choose a random element from an array
#define random_choice(state, arr) arr[random_range(state, 0, ARRAY_LEN(arr))]

// Shuffle arr in place. element_size is `sizeof arr[0]`.
void random_shuffle(RandomState *state, size_t element_size, void *arr, size_t arr_len);

// -------------------- real-valued distributions  -------------------

// Get a uniformly distributed random number in the range [0, 1).
double random_real(RandomState *state);

// Get a uniformly distributed random number in the range [min, max).
double random_real_range(RandomState *state, double min, double max);

#endif // _RANDOM_HEADER