#include "protocolServer-SM-Tests.h"
#include "protocolServer-SM-Tests-Functions.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#define TESTING_SUITE_NAME "protocol_server_sm_suite"

#define NUM_ARGS 5
#define TEST_TO_RUN_INDEX 4

Suite *protocol_server_sm_suite(void);

static struct server_data_sm sd;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static char test_to_run;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(int argc, char *argv[])
{
    int      number_failed;
    Suite   *s;
    SRunner *sr;

    if(argc != NUM_ARGS)
    {
        return EXIT_FAILURE;
    }

    test_to_run             = argv[TEST_TO_RUN_INDEX][0];
    argv[TEST_TO_RUN_INDEX] = NULL;
    argc                    = NUM_ARGS - 1;
    printf("new argc: %d\n", argc);

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
START_TEST(test_connect_correct_passcode)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_correct_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(EXIT_SUCCESS, result);
}

// SEND THE INCORRECT PASSCODE TO THE SERVER ONCE.
START_TEST(test_connect_incorrect_passcode_once)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = connect_incorrect_passcode(sd.argc, sd.argv);
    ck_assert_int_eq(EXIT_FAILURE, result);
}

// CLIENT CONNECTS TO SERVER BEFORE SENDING /q
START_TEST(test_client_connects_before_sending_q)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = client_connect_normal(sd.argc, sd.argv);
    ck_assert_int_eq(EXIT_SUCCESS, result);
}

// CLIENT CONNECTS TO SERVER AFTER SENDING /q
START_TEST(test_client_connects_after_sending_q)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    int result;
    result = client_connect_after_sending_q(sd.argc, sd.argv);
    ck_assert_int_eq(EXIT_FAILURE, result);
}

Suite *protocol_server_sm_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s       = suite_create(TESTING_SUITE_NAME);
    tc_core = tcase_create("Core");

    switch(test_to_run)
    {
        case '1':
            tcase_add_test(tc_core, test_connect_correct_passcode);
            break;
        case '2':
            tcase_add_test(tc_core, test_connect_incorrect_passcode_once);
            break;
        case '3':
            tcase_add_test(tc_core, test_client_connects_before_sending_q);
            break;
        case '4':
            tcase_add_test(tc_core, test_client_connects_after_sending_q);
            break;
        default:
            printf("Invalid test_to_run\n");
            break;
    }

    suite_add_tcase(s, tc_core);

    return s;
}
