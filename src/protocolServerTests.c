#include "../include/buildingClients.h"
#include <check.h>
#include <stdlib.h>

Suite *protocol_limits_suite(void);

struct server_data
{
    int    argc;
    char **argv;
};

struct server_data sd;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(int argc, char *argv[])
{
    int      number_failed;    // The number of tests failed
    Suite   *s;                // The testing suite to run
    SRunner *sr;               // The suite runner
    sd.argc = argc;
    sd.argv = argv;
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

// Test for minimum number in the version field.
// START_TEST(test_protocol_version_min_number)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
//{
//    int x = 2;
//    ck_assert_int_eq(2, x);
//}
//
START_TEST(test_format)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_format(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_username_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_username_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_userlist_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_userlist_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_whisper_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_whisper_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_help_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_help_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_the_server_limit)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = create_client_max(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_server_chat_message)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_client_chat_message(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

START_TEST(test_same_username)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x = check_username_already_exists(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
}

// Create test suite
Suite *protocol_limits_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s       = suite_create("Protocol Limits");
    tc_core = tcase_create("Core");

    // Add test cases to the test suite
    tcase_add_test(tc_core, test_format);
    tcase_add_test(tc_core, test_username_command);
    tcase_add_test(tc_core, test_userlist_command);
    tcase_add_test(tc_core, test_whisper_command);
    tcase_add_test(tc_core, test_help_command);
    tcase_add_test(tc_core, test_the_server_limit);
    tcase_add_test(tc_core, test_same_username);

    //    tcase_add_test(tc_core, test_protocol_version_min_number);
    tcase_add_test(tc_core, test_server_chat_message);

    suite_add_tcase(s, tc_core);
    return s;
}
