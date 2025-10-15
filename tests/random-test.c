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

    // Test creating multiple RNGs
    RandomState *rng2 = random_make();
    REQUIRE_NOT_NULL(rng2);
    CHECK_NE((intptr_t)rng2, (intptr_t)tau->rng);
    random_destroy(rng2);
}

TEST_F(RandomTestFixture, random_range)
{
    uint64_t min = 10, max = 20;
    int counts[10] = {0}; // Track distribution

    // min <= n < max
    for (size_t i = 0; i < 10000; i++)
    {
        uint64_t res = random_range(tau->rng, min, max);
        CHECK_GE(res, min, "result not in valid range");
        CHECK_LT(res, max, "result not in valid range");
        counts[res - min]++;
    }

    // Check rough distribution (not perfect but should be somewhat uniform)
    for (int i = 0; i < 10; i++)
    {
        CHECK_GT(counts[i], 950); // Should get roughly 1000 each, allow some variance
    }

    // Test edge cases
    uint64_t res = random_range(tau->rng, 0, 1);
    CHECK_EQ(res, 0);

    res = random_range(tau->rng, UINT64_MAX - 1, UINT64_MAX);
    CHECK_EQ(res, UINT64_MAX - 1);
}

TEST_F(RandomTestFixture, random_real)
{
    // Test range [0,1)
    for (size_t i = 0; i < 1000; i++)
    {
        double res = random_real(tau->rng);
        CHECK_GE(res, 0.0);
        CHECK_LT(res, 1.0);
    }
}

TEST_F(RandomTestFixture, random_real_range)
{
    double min = -10.5, max = 20.7;

    // Test min <= n < max
    for (size_t i = 0; i < 1000; i++)
    {
        double res = random_real_range(tau->rng, min, max);
        CHECK_GE(res, min);
        CHECK_LT(res, max);
    }

    // Test with very small range
    double res = random_real_range(tau->rng, 1.0, 1.0000001);
    CHECK_GE(res, 1.0);
    CHECK_LT(res, 1.0000001);
}

TEST_F(RandomTestFixture, random_choice)
{
    int arr[] = {1, 2, 3, 4, 5};
    int counts[5] = {0};

    // Test distribution
    for (size_t i = 0; i < 5000; i++)
    {
        int choice = random_choice(tau->rng, arr);
        CHECK_GE(choice, 1);
        CHECK_LE(choice, 5);
        counts[choice - 1]++;
    }

    // Each number should be chosen roughly 1000 times
    for (int i = 0; i < 5; i++)
    {
        CHECK_GT(counts[i], 900); // Allow some variance
        CHECK_LT(counts[i], 1100);
    }
}

TEST_F(RandomTestFixture, random_shuffle)
{
    int arr[] = {1, 2, 3, 4, 5};
    int original[] = {1, 2, 3, 4, 5};
    int equal_count = 0;

    // Test multiple shuffles
    for (size_t i = 0; i < 100; i++)
    {
        // Copy original array
        int test_arr[5];
        memcpy(test_arr, arr, sizeof(arr));

        // Shuffle
        random_shuffle(tau->rng, sizeof(int), test_arr, 5);

        // Verify all elements still exist
        int found[5] = {0};
        for (int j = 0; j < 5; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                if (test_arr[j] == original[k])
                {
                    found[k] = 1;
                    break;
                }
            }
        }
        for (int j = 0; j < 5; j++)
        {
            CHECK_EQ(found[j], 1);
        }

        // Count how many times we get the original arrangement
        bool is_equal = true;
        for (int j = 0; j < 5; j++)
        {
            if (test_arr[j] != original[j])
            {
                is_equal = false;
                break;
            }
        }
        if (is_equal)
            equal_count++;
    }

    // Shouldn't get original arrangement too often
    CHECK_LT(equal_count, 10);

    // Test edge cases
    int single[] = {1};
    random_shuffle(tau->rng, sizeof(int), single, 1);
    CHECK_EQ(single[0], 1);

    int empty[] = {};
    random_shuffle(tau->rng, sizeof(int), empty, 0);
    // Should not crash
}
