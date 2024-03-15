#include "../include/protocolClientTests.h"
#include "../include/protocolMockServer.h"
#include <check.h>
#include <stdlib.h>
#include <unistd.h>

struct params
{
    int client_fd;
    int server_fd;
};

static struct params params;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

Suite *protocol_client_suite(void);

// Test for client connection to server
START_TEST(test_client_connection)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_ne(-1, params.client_fd);    // Fail if client_fd == -1
}

// Test for writing correct message format
START_TEST(test_client_message_format)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.client_fd);
}

// Test for /w command
START_TEST(test_client_whisper_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.client_fd);
}

// Test for /h command
START_TEST(test_client_help_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.client_fd);
}

// Test for /u command
START_TEST(test_client_username_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.client_fd);
}

// Test for /ul command
START_TEST(test_client_userlist_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_eq(0, params.client_fd);
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

int testProtocolClient(void)
{
    int                     number_failed;      // The number of tests failed
    int                     server_fd;          // The mock server socket
    int                     client_fd;          // The client socket
    struct sockaddr_storage client_addr;        // The client address
    socklen_t               client_addr_len;    // The client address length
    Suite                  *s;                  // The testing suite to run
    SRunner                *sr;                 // The suite runner

    // Create mock server and accept a connection
    server_fd = create_server();
    client_fd = accept_connection(server_fd, &client_addr, &client_addr_len);

    // Pass parameter into struct to be used by test cases
    params.client_fd = client_fd;
    params.server_fd = server_fd;

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
    close(client_fd);
    close(server_fd);

    // Return status
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
