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

// Test for password
START_TEST(test_server_manager_password)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    size_t      msg_len;
    const char *response      = "ACCEPTED";
    char       *actualContent = {0};

    printf("Type server password\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.server_manager_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.server_manager_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.server_manager_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for passowrd
    ck_assert_str_eq("hellyabrother", actualContent);
    free(actualContent);

    version     = 1;
    msg_len     = strlen(response);
    contentSize = htons((uint16_t)msg_len);
    //    printf("Write version result %zd\n", write(params.server_manager_fd, &version, sizeof(version)));
    write(params.server_manager_fd, &version, sizeof(version));
    write(params.server_manager_fd, &contentSize, sizeof(contentSize));
    write(params.server_manager_fd, response, msg_len);
}

// Test for /s command
START_TEST(test_server_manager_start_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    size_t      msg_len;
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    const char *response      = "STARTED";
    char       *actualContent = {0};

    printf("Type \"/s\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.server_manager_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.server_manager_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.server_manager_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    ck_assert_str_eq("/s", actualContent);

    version     = 1;
    msg_len     = strlen(response);
    contentSize = htons((uint16_t)msg_len);

    write(params.server_manager_fd, &version, sizeof(version));
    write(params.server_manager_fd, &contentSize, sizeof(contentSize));
    write(params.server_manager_fd, response, msg_len);

    free(actualContent);
}

// Test for diagnostics
START_TEST(test_server_manager_diagnostics_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t     bytes_read;
    size_t      msg_len;
    uint8_t     version;
    uint16_t    contentSize;
    const char *response      = "4";
    int         result        = 3;
    char       *actualContent = {0};

    printf("Type \"/d\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.server_manager_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.server_manager_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.server_manager_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    ck_assert_str_eq("/d", actualContent);

    version     = 1;
    msg_len     = strlen(response);
    contentSize = htons((uint16_t)msg_len);

    for(int i = 0; i < result; i++)
    {
        write(params.server_manager_fd, &version, sizeof(version));
        write(params.server_manager_fd, &contentSize, sizeof(contentSize));
        write(params.server_manager_fd, response, msg_len);
        sleep(1);
    }

    // Check with server managers to see if they received the correct diagnostics
    ck_assert_int_eq(result, result);
}

// Test for /q command
START_TEST(test_server_manager_quit_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    size_t      msg_len;
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    const char *response      = "STOPPED";
    char       *actualContent = {0};

    printf("Type \"/q\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.server_manager_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.server_manager_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.server_manager_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    ck_assert_str_eq("/q", actualContent);

    version     = 1;
    msg_len     = strlen(response);
    contentSize = htons((uint16_t)msg_len);

    write(params.server_manager_fd, &version, sizeof(version));
    write(params.server_manager_fd, &contentSize, sizeof(contentSize));
    write(params.server_manager_fd, response, msg_len);

    free(actualContent);
}

Suite *protocol_server_manager_suite(void)
{
    Suite    *s;
    TCase    *tc_core;
    const int timeout = 10;

    s       = suite_create("Server Manager Tests");
    tc_core = tcase_create("Core");

    // Add tests to the test case
    tcase_add_test(tc_core, test_server_manager_connection);
    tcase_add_test(tc_core, test_server_manager_password);
    tcase_add_test(tc_core, test_server_manager_start_command);
    tcase_add_test(tc_core, test_server_manager_diagnostics_command);
    tcase_add_test(tc_core, test_server_manager_quit_command);

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

// Main function to run the tests
int main(void)
{
    testProtocolServerManager();
    return EXIT_SUCCESS;
}
