#include "protocolLimitsTests.h"
#include <check.h>
#include <stdlib.h>

Suite *protocol_limits_suite(void);

// Test for minimum number in the version field.
START_TEST(test_protocol_version_min_number)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = 2;
    ck_assert_int_eq(1, x);
}

// Test for maximum number in the version field.
START_TEST(test_protocol_version_max_number)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = 2;
    ck_assert_int_eq(1, x);
}

// Test for minimum number in the content size field.
START_TEST(test_protocol_content_size_min_number)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = 2;
    ck_assert_int_eq(1, x);
}

// Test for maximum number in the content size field.
START_TEST(test_protocol_content_size_max_number)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = 2;
    ck_assert_int_eq(1, x);
}

// Test for minimum chars in content field.
START_TEST(test_protocol_content_min_chars)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = 2;
    ck_assert_int_eq(1, x);
}

// Test for minimum chars in content field.
START_TEST(test_protocol_content_max_chars)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int y = 2;
    ck_assert_int_eq(2, y);
}

// Create test suite
Suite *protocol_limits_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s       = suite_create("Protocol Limits");
    tc_core = tcase_create("Core");

    // Add test cases to the test suite
    tcase_add_test(tc_core, test_protocol_version_min_number);
    tcase_add_test(tc_core, test_protocol_version_max_number);

    tcase_add_test(tc_core, test_protocol_content_size_min_number);
    tcase_add_test(tc_core, test_protocol_content_size_max_number);

    tcase_add_test(tc_core, test_protocol_content_min_chars);
    tcase_add_test(tc_core, test_protocol_content_max_chars);

    suite_add_tcase(s, tc_core);
    return s;
}

int testProtocolLimits(void)
{
    int      number_failed;    // The number of tests failed
    Suite   *s;                // The testing suite to run
    SRunner *sr;               // The suite runner

    // Create test suite
    s = protocol_limits_suite();

    // Create test runner
    sr = srunner_create(s);

    // Run the tests
    srunner_run_all(sr, CK_NORMAL);

    // Get number of failed tests
    number_failed = srunner_ntests_failed(sr);

    // Clean up
    srunner_free(sr);

    // Return status
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
