#include "tau/tau.h"
#include "utils.c"

TEST(Utils, n_cores)
{
    int cores = n_cores();
#ifdef ENABLE_THREADS
    CHECK_GT(cores, 0);
#else
    CHECK_EQ(cores, 1);
#endif
}

TEST(Utils, log_levels)
{
    // Test that log levels are ordered correctly
    CHECK_LT(U_LOG_DEBUG, U_LOG_INFO);
    CHECK_LT(U_LOG_INFO, U_LOG_WARN);
    CHECK_LT(U_LOG_WARN, U_LOG_ERROR);
}

TEST(Utils, str_lower_basic)
{
    const char *input = "Hello World!";
    char *result = str_lower(input);
    CHECK_STREQ(result, "hello world!");
}

TEST(Utils, str_lower_empty)
{
    const char *input = "";
    char *result = str_lower(input);
    CHECK_STREQ(result, "");
}

TEST(Utils, str_lower_null)
{
    char *result = str_lower(NULL);
    CHECK_NULL(result);
}

TEST(Utils, str_lower_mixed)
{
    const char *input = "MiXeD CaSe 123!@#";
    char *result = str_lower(input);
    CHECK_STREQ(result, "mixed case 123!@#");
}

TEST(Utils, str_lower_already_lower)
{
    const char *input = "already lowercase";
    char *result = str_lower(input);
    CHECK_STREQ(result, "already lowercase");
}

TEST(Utils, str_lower_all_upper)
{
    const char *input = "ALL UPPER CASE";
    char *result = str_lower(input);
    CHECK_STREQ(result, "all upper case");
}

TEST(Utils, array_len_macro)
{
    int arr[] = {1, 2, 3, 4, 5};
    size_t len = ARRAY_LEN(arr);
    CHECK_EQ(len, 5);

    char str[] = "test";
    len = ARRAY_LEN(str);
    CHECK_EQ(len, 5); // includes null terminator
}
