#include "tau/tau.h"
#include "another-test.c"

TAU_MAIN()

TEST(tau_test, sanity_check)
{
    CHECK(1); // does not fail
}

TEST(tau_test, bar1)
{
    int a = 42;
    int b = 13;
    CHECK_GE(a, b); // pass :)
    // CHECK_LE(b, 8); // fail - Test suite not aborted
}

TEST(tau_test, bar2)
{
    char *a = "foo";
    char *b = "foobar";
    REQUIRE_STREQ(a, a); // pass :)
    // REQUIRE_STREQ(a, b); // fail - Test suite aborted
}