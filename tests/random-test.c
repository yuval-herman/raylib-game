#include "tau/tau.h"
#include "random.c"

struct RandomTestFixture
{
    RandomState *rng;
};

TEST_F_SETUP(RandomTestFixture)
{
    tau->rng = random_make();
    random_seed(tau->rng, 0, 0); // for deterministic tests
}

TEST_F_TEARDOWN(RandomTestFixture)
{
    random_destroy(tau->rng);
}

TEST_F(RandomTestFixture, random_make)
{
    REQUIRE_NOT_NULL(tau->rng);
}

TEST_F(RandomTestFixture, random_range)
{
    uint64_t min = 10, max = 20;
    // min <= n < max
    for (size_t i = 0; i < 1000; i++)
    {
        uint64_t res = random_range(tau->rng, min, max);
        CHECK_GE(res, min, "result not in valid range");
        CHECK_LT(res, max, "result not in valid range");
    }
}