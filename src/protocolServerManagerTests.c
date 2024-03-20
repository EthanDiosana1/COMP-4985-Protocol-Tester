#include "../include/protocolServerManagerTests.h"
#include "../include/protocolMockServer.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct params
{
    int server_manager_fd;
    int server_fd;
};

static struct params params;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

Suite *protocol_server_manager_suite(void);

// Test for server manager connection to server
START_TEST(test_server_manager_connection)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ck_assert_int_ne(-1, params.server_manager_fd);    // Fail if client_fd == -1
}

// Test for /s command
START_TEST(test_server_manager_start_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    char       *actualContent = {0};
    char       *token;
    char       *savePtr;
    const char *delimiter = " ";

    printf("Type \"/s 192.168.0.1\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.server_manager_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.server_manager_fd, &contentSize, sizeof(contentSize));

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.server_manager_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for whisper command
    token = strtok_r(actualContent, delimiter, &savePtr);
    ck_assert_str_eq("/s", token);

    // Check for username
    token = strtok_r(NULL, delimiter, &savePtr);
    ck_assert_str_eq("192.168.0.1\n", token);

    free(actualContent);
}

Suite *protocol_server_manager_suite(void)
{
    Suite    *s;
    TCase    *tc_core;
    const int timeout = 10;

    s       = suite_create("Client Tests");
    tc_core = tcase_create("Core");

    // Add tests to the test case
    tcase_add_test(tc_core, test_server_manager_connection);
    tcase_add_test(tc_core, test_server_manager_start_command);

    // Set timeout timer to 10 seconds
    tcase_set_timeout(tc_core, timeout);

    // Add test case to the test suite
    suite_add_tcase(s, tc_core);

    return s;
}

int testProtocolServerManager(void)
{
    int                     number_failed;              // The number of tests failed
    int                     server_fd;                  // The mock server socket
    int                     server_manager_fd;          // The server manager socket
    struct sockaddr_storage server_manager_addr;        // The server manager address
    socklen_t               server_manager_addr_len;    // The server manager address length
    Suite                  *s;                          // The testing suite to run
    SRunner                *sr;                         // The suite runner

    // Create mock server and accept a connection
    server_fd         = create_server();
    server_manager_fd = accept_connection(server_fd, &server_manager_addr, &server_manager_addr_len);

    // Pass parameter into struct to be used by test cases
    params.server_manager_fd = server_manager_fd;
    params.server_fd         = server_fd;

    // Create test suite
    s = protocol_server_manager_suite();

    // Create test runner
    sr = srunner_create(s);

    // Run the tests
    srunner_run_all(sr, CK_NORMAL);

    // Get number of failed tests
    number_failed = srunner_ntests_failed(sr);

    // Clean up
    srunner_free(sr);
    close(server_manager_fd);
    close(server_fd);

    // Return status
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
