#include "../include/protocolClientTests.h"
#include "../include/protocolMockServer.h"
#include <check.h>
#include <stdlib.h>
#include <unistd.h>

struct params
{
    int test_sockfd;
};

static struct params params;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

Suite *protocol_client_suite(void);

// Test for client connection to server
START_TEST(test_client_connection)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Test for writing correct message format
START_TEST(test_client_message_format)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Test for /w command
START_TEST(test_client_whisper_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Test for /h command
START_TEST(test_client_help_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Test for /u command
START_TEST(test_client_username_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Test for /ul command
START_TEST(test_client_userlist_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.test_sockfd);
}

// Create test suite
Suite *protocol_client_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s       = suite_create("Client Tests");
    tc_core = tcase_create("Core");

    // Add tests to the test case
    tcase_add_test(tc_core, test_client_connection);
    tcase_add_test(tc_core, test_client_message_format);
    tcase_add_test(tc_core, test_client_whisper_command);
    tcase_add_test(tc_core, test_client_help_command);
    tcase_add_test(tc_core, test_client_username_command);
    tcase_add_test(tc_core, test_client_userlist_command);

    // Add test case to the test suite
    suite_add_tcase(s, tc_core);

    return s;
}

int testProtocolClient(int sockfd)
{
    int      number_failed;    // The number of tests failed
    int      server_fd;        // The mock server socket
    Suite   *s;                // The testing suite to run
    SRunner *sr;               // The suite runner

    // Pass parameter into struct to be used by test cases
    params.test_sockfd = sockfd;

    // Create mock server
    server_fd = create_server();

    // Create test suite
    s = protocol_client_suite();

    // Create test runner
    sr = srunner_create(s);

    // Run the tests
    srunner_run_all(sr, CK_NORMAL);

    // Get number of failed tests
    number_failed = srunner_ntests_failed(sr);

    // Clean up
    srunner_free(sr);
    close(server_fd);

    // Return status
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
