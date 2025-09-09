#ifndef _RANDOM_HEADER
#define _RANDOM_HEADER

#include <stdint.h>

typedef struct RandomState RandomState;
typedef __uint128_t pcg128_t;

// Creates a new, unseeded RandomState.
// Requires a subsequent call to `random_seed`.
RandomState *random_make();

// Creates a new RandomState and seeds it using the current time and a stack
// address for entropy.
RandomState *random_make_seed();

// Seeds an existing RandomState with a 128-bit state and a 128-bit sequence selector.
void random_seed(RandomState *state, pcg128_t initstate, pcg128_t initseq);

void random_destroy(RandomState *state);

/**************************************************
 * INTEGER GENERATION
 ***************************************************/

/**
 * @brief Generates a random 64-bit unsigned integer in the full range [0, UINT64_MAX].
 * @param state A pointer to the RandomState.
 * @return A random uint64_t.
 */
uint64_t random_uint64(RandomState *state);

/**
 * @brief Generates a random 64-bit unsigned integer in the range [0, barrier).
 * @param state A pointer to the RandomState.
 * @param barrier The exclusive upper bound.
 * @return A random uint64_t less than barrier.
 */
uint64_t random_uint64_upto(RandomState *state, uint64_t barrier);

/**
 * @brief Generates a random 64-bit unsigned integer in the range [min, max].
 * @param state A pointer to the RandomState.
 * @param min The inclusive lower bound.
 * @param max The inclusive upper bound.
 * @return A random uint64_t within the specified range.
 * @precondition max >= min
 */
uint64_t random_uint64_range(RandomState *state, uint64_t min, uint64_t max);

/**************************************************
 * FLOATING-POINT GENERATION
 ***************************************************/

/**
 * @brief Generates a random double in the range [0.0, 1.0).
 * Provides 53 bits of precision. The value 1.0 is never returned.
 * @param state A pointer to the RandomState.
 * @return A random double.
 */
double random_double(RandomState *state);

/**
 * @brief Generates a random float in the range [0.0f, 1.0f).
 * Provides 24 bits of precision. The value 1.0f is never returned.
 * @param state A pointer to the RandomState.
 * @return A random float.
 */
float random_float(RandomState *state);

/**
 * @brief Generates a random double in the range [min, max).
 * @param state A pointer to the RandomState.
 * @param min The inclusive lower bound.
 * @param max The exclusive upper bound.
 * @return A random double within the specified range.
 * @precondition max >= min
 */
double random_double_range(RandomState *state, double min, double max);

/**
 * @brief Generates a random float in the range [min, max).
 * @param state A pointer to the RandomState.
 * @param min The inclusive lower bound.
 * @param max The exclusive upper bound.
 * @return A random float within the specified range.
 * @precondition max >= min
 */
float random_float_range(RandomState *state, float min, float max);

#endif // _RANDOM_HEADER