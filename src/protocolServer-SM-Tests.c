#include "protocolServer-SM-Tests.h"
#include "protocolServer-SM-Tests-Functions.h"
#include <check.h>
#include <stdlib.h>

#define TESTING_SUITE_NAME "protocol_server_sm_suite"

#define TEST1 test_connect_correct_passcode
#define TEST2 test_connect_incorrect_passcode_once
#define TEST3 test_connect_incorrect_passcode_5_times
#define TEST4 test_client_connects_before_sending_q
#define TEST5 test_client_connects_after_sending_q

Suite *protocol_server_sm_suite(void);

static struct server_data_sm sd;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(int argc, char *argv[])
{
    int      number_failed;
    Suite   *s;
    SRunner *sr;
    sd.argc = argc;
    sd.argv = argv;

    // Create test suite
    s = protocol_server_sm_suite();

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

// SEND THE CORRECT PASSCODE TO THE SERVER.
START_TEST(TEST1)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(0, result);
}

// SEND THE INCORRECT PASSCODE TO THE SERVER ONCE.
START_TEST(TEST2)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(0, result);
}

// SEND THE INCORRECT PASSCODE TO THE SERVER 5 TIMES.
START_TEST(TEST3)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(0, result);
}

// CLIENT CONNECTS TO SERVER BEFORE SENDING /q
START_TEST(TEST4)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(0, result);
}

// CLIENT CONNECTS TO SERVER AFTER SENDING /q
START_TEST(TEST5)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(0, result);
}

Suite *protocol_server_sm_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s       = suite_create(TESTING_SUITE_NAME);
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, TEST1);
    tcase_add_test(tc_core, TEST2);
    tcase_add_test(tc_core, TEST3);
    tcase_add_test(tc_core, TEST4);
    tcase_add_test(tc_core, TEST5);

    suite_add_tcase(s, tc_core);

    return s;
}
