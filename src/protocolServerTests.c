#include "../include/buildingClients.h"
#include <check.h>
#include <stdio.h>
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

START_TEST(test_format)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Format======================\n");
    x = check_format(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_username_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Username Command======================\n");
    x = check_username_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_userlist_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Userlist Command======================\n");
    x = check_userlist_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_whisper_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Whisper Command======================\n");
    x = check_whisper_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_help_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Help Command======================\n");
    x = check_help_command(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_the_server_limit)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Server Limit======================\n");
    x = create_client_max(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_server_chat_message)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Chat======================\n");
    x = check_client_chat_message(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_same_username)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Same Usernames======================\n");
    x = check_username_already_exists(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_version_min)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Version Min======================\n");
    x = check_version_min(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_version_max)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Version Max======================\n");
    printf("If this test passes, it may still require editing, based on the Testing Version Number test.\n");
    printf("If this test fails, but passes on the Testing Version Number test you are good.\n");
    x = check_version_max(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_version_num)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Version Num======================\n");
    printf("If this test passes, that means you closed the connection on an incompatible version.\n");
    printf("If this test fails, then the versions do not match, we are still getting a response on incompatible version types.\n");
    x = check_version_num(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_content_min)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Content Min======================\n");
    x = check_content_size_min(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
}

START_TEST(test_content_max)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int x;
    printf("\n=====================Testing Content Max======================\n");
    printf("If this test fails, it is likely because the buffer on the server side may be too small.\n");
    x = check_content_size_max(sd.argc, sd.argv);
    ck_assert_int_eq(0, x);
    printf("\n=========================================================\n");
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
    tcase_add_test(tc_core, test_version_min);
    tcase_add_test(tc_core, test_version_max);
    tcase_add_test(tc_core, test_version_num);
    tcase_add_test(tc_core, test_content_min);
    tcase_add_test(tc_core, test_content_max);

    tcase_add_test(tc_core, test_username_command);
    tcase_add_test(tc_core, test_same_username);
    tcase_add_test(tc_core, test_userlist_command);
    tcase_add_test(tc_core, test_whisper_command);
    tcase_add_test(tc_core, test_help_command);

    tcase_add_test(tc_core, test_the_server_limit);
    tcase_add_test(tc_core, test_server_chat_message);

    suite_add_tcase(s, tc_core);
    return s;
}
