#include "random.h"
#include "pcg_variants.h"

#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h> // For UINT64_MAX

struct RandomState
{
    pcg64_random_t pcg_state;
};

void random_seed(RandomState *state, pcg128_t initstate, pcg128_t initseq)
{
    pcg64_srandom_r(&state->pcg_state, initstate, initseq);
}

RandomState *random_make()
{
    return malloc(sizeof(RandomState));
}

RandomState *random_make_seed()
{
    RandomState *r = malloc(sizeof *r);
    // Seed using the current time and the address of the allocated struct.
    // The pointer address adds a bit of entropy that changes between runs.
    random_seed(r, (pcg128_t)time(NULL), (intptr_t)&r);

    return r;
}

void random_destroy(RandomState *state)
{
    free(state);
}

uint64_t random_uint64(RandomState *state)
{
    return pcg64_random_r(&state->pcg_state);
}

uint64_t random_uint64_upto(RandomState *state, uint64_t barrier)
{
    return pcg64_boundedrand_r(&state->pcg_state, barrier);
}

uint64_t random_uint64_range(RandomState *state, uint64_t min, uint64_t max)
{
    // This handles the edge case where the user requests the full 64-bit range.
    // `max - min` would overflow, but `UINT64_MAX` signals this exact case.
    uint64_t range = max - min;
    if (range == UINT64_MAX)
    {
        return random_uint64(state);
    }

    // pcg64_boundedrand_r generates a number in [0, bound - 1].
    // To get a number in [min, max], we need to generate an offset
    // in the range [0, max - min]. The number of values is (max - min) + 1.
    // So, we use that as the bound.
    return min + random_uint64_upto(state, range + 1);
}

double random_double(RandomState *state)
{
    return (random_uint64(state) >> 11) * 0x1.0p-53;
}

float random_float(RandomState *state)
{
    return (random_uint64(state) >> 40) * 0x1.0p-24f;
}

double random_double_range(RandomState *state, double min, double max)
{
    // Standard linear interpolation: scale the [0, 1) result to the desired range.
    return min + (max - min) * random_double(state);
}

float random_float_range(RandomState *state, float min, float max)
{
    return min + (max - min) * random_float(state);
}