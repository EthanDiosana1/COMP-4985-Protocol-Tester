#include "../include/protocolClientTests.h"
#include "../include/protocolMockServer.h"
#include <check.h>
#include <stdio.h>
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
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    char       *actualContent   = {0};
    const char *expectedContent = "Hello World\n";

    printf("Type \"Hello World\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);
    ck_assert_int_eq(1, version);    // Check version

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);
    ck_assert_int_eq(strlen(expectedContent), contentSize);    // Check content size

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';
    ck_assert_str_eq(expectedContent, actualContent);    // Check content

    free(actualContent);
}

// Test for /w command
START_TEST(test_client_whisper_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    char       *actualContent = {0};
    char       *token;
    char       *savePtr;
    const char *delimiter = " ";

    printf("Type \"/w c1 Hi\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for whisper command
    token = strtok_r(actualContent, delimiter, &savePtr);
    ck_assert_str_eq("/w", token);

    // Check for username
    token = strtok_r(NULL, delimiter, &savePtr);
    ck_assert_str_eq("c1", token);

    // Check for content
    token = strtok_r(NULL, delimiter, &savePtr);
    ck_assert_str_eq("Hi\n", token);

    free(actualContent);
}

// Test for /h command
START_TEST(test_client_help_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t  bytes_read;
    uint8_t  version;
    uint16_t contentSize;
    char    *actualContent = {0};

    printf("Type \"/h\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for help command
    ck_assert_str_eq("/h\n", actualContent);

    free(actualContent);
}

// Test for /u command
START_TEST(test_client_username_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t     bytes_read;
    uint8_t     version;
    uint16_t    contentSize;
    char       *actualContent = {0};
    char       *token;
    char       *savePtr;
    const char *delimiter = " ";

    printf("Type \"/u newName\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for username command
    token = strtok_r(actualContent, delimiter, &savePtr);
    ck_assert_str_eq("/u", token);

    // Check for a new username
    token = strtok_r(NULL, delimiter, &savePtr);
    ck_assert_str_eq("newName\n", token);

    free(actualContent);
}

// Test for /ul command
START_TEST(test_client_userlist_command)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t  bytes_read;
    uint8_t  version;
    uint16_t contentSize;
    char    *actualContent = {0};

    printf("Type \"/ul\"\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for user list command
    ck_assert_str_eq("/ul\n", actualContent);

    free(actualContent);
}

// Test for writing minimum fields
START_TEST(test_client_write_min_fields)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t  bytes_read;
    uint8_t  version;
    uint16_t contentSize;
    char    *actualContent = {0};

    printf("Press enter\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    ck_assert_int_eq(1, version);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    ck_assert_int_eq(1, contentSize);    // Check content size

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for content
    ck_assert_str_eq("\n", actualContent);

    free(actualContent);
}

// Test for writing maximum fields
START_TEST(test_client_write_max_fields)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    ssize_t  bytes_read;
    uint8_t  version;
    uint16_t contentSize;
    char    *actualContent = {0};
    char     msg[UINT16_MAX];

    for(int i = 0; i < UINT16_MAX - 2; i++)
    {
        msg[i] = 'a';
    }

    msg[UINT16_MAX - 1] = '\0';

    printf("Print maximum message\n");

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &version, sizeof(version));
    } while(bytes_read < 1);

    ck_assert_int_eq(UINT8_MAX, version);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    ck_assert_int_eq(UINT16_MAX, contentSize);    // Check content size

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for content
    ck_assert_str_eq(msg, actualContent);

    free(actualContent);
}

// Test for reading minimum fields
START_TEST(test_client_read_min_fields)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    size_t      msg_len;
    ssize_t     bytes_read;
    uint8_t     write_version;
    uint8_t     read_version;
    uint16_t    contentSize;
    char       *actualContent = {0};
    const char *msg           = "1";

    printf("Type received message\n");

    write_version = 1;
    msg_len       = strlen(msg);
    contentSize   = htons((uint16_t)msg_len);

    write(params.client_fd, &write_version, sizeof(write_version));
    write(params.client_fd, &contentSize, sizeof(contentSize));
    write(params.client_fd, msg, msg_len);

    // Constantly read until bytes are read
    do
    {
        bytes_read = read(params.client_fd, &read_version, sizeof(read_version));
    } while(bytes_read < 1);

    ck_assert_int_eq(1, read_version);

    // Read content size
    read(params.client_fd, &contentSize, sizeof(contentSize));

    contentSize = ntohs(contentSize);

    ck_assert_int_eq(2, contentSize);    // Check content size

    // Allocate memory for the content string
    actualContent = malloc(contentSize + 1);
    if(actualContent == NULL)
    {
        return;
    }

    // Read content string
    read(params.client_fd, actualContent, contentSize);
    actualContent[contentSize] = '\0';

    // Check for content
    ck_assert_str_eq("1\n", actualContent);

    free(actualContent);
}

// Test for reading maximum fields
START_TEST(test_client_read_max_fields)    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
    size_t   msg_len;
    uint8_t  version;
    uint16_t contentSize;
    char     msg[UINT16_MAX];

    for(int i = 0; i < UINT16_MAX - 2; i++)
    {
        msg[i] = 'a';
    }

    msg[UINT16_MAX - 1] = '\0';

    version     = UINT8_MAX;
    msg_len     = UINT16_MAX;
    contentSize = htons((uint16_t)msg_len);

    write(params.client_fd, &version, sizeof(version));
    write(params.client_fd, &contentSize, sizeof(contentSize));
    write(params.client_fd, msg, strlen(msg));

    ck_assert_int_eq(UINT16_MAX, contentSize);    // sample assert to get rid of timeout
    // check with client program to see if they received correctly
}

// Create test suite
Suite *protocol_client_suite(void)
{
    Suite    *s;
    TCase    *tc_core;
    const int timeout = 30;

    s       = suite_create("Client Tests");
    tc_core = tcase_create("Core");

    // Add tests to the test case
    tcase_add_test(tc_core, test_client_connection);
    tcase_add_test(tc_core, test_client_message_format);
    tcase_add_test(tc_core, test_client_whisper_command);
    tcase_add_test(tc_core, test_client_help_command);
    tcase_add_test(tc_core, test_client_username_command);
    tcase_add_test(tc_core, test_client_userlist_command);
    tcase_add_test(tc_core, test_client_read_min_fields);
    tcase_add_test(tc_core, test_client_read_max_fields);
    tcase_add_test(tc_core, test_client_write_min_fields);
    tcase_add_test(tc_core, test_client_write_max_fields);

    // Set timeout timer to 10 seconds
    tcase_set_timeout(tc_core, timeout);

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

// Main function to run the tests
int main(void)
{
    testProtocolClient();
    return EXIT_SUCCESS;
}
