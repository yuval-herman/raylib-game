#include "random.h"
#include "pcg_variants.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>

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

uint64_t random_range(RandomState *state, uint64_t min, uint64_t max)
{
    assert(max > min);
    uint64_t width = max - min;

    return min + pcg64_boundedrand_r(&state->pcg_state, width);
}

void swap(void *a, void *b, size_t len)
{
    unsigned char *p = a, *q = b, tmp;
    for (size_t i = 0; i != len; ++i)
    {
        tmp = p[i];
        p[i] = q[i];
        q[i] = tmp;
    }
}

void random_shuffle(RandomState *state, size_t element_size, void *arr, size_t arr_len)
{
    unsigned char *p_arr = arr;

    if (arr_len <= 1)
        return;

    for (size_t i = arr_len - 1; i > 0; i--)
    {
        size_t j = pcg64_boundedrand_r(&state->pcg_state, i + 1);

        swap(p_arr + i * element_size, p_arr + j * element_size, element_size);
    }
}

double random_real(RandomState *state)
{
    return ldexp(pcg64_random_r(&state->pcg_state), -64);
}

double random_real_range(RandomState *state, double min, double max)
{
    return min + (max - min) * random_real(state);
}
