/** This file runs a server manager.
 *
 * It is used to test how a server
 * handles server managers.
 *
 * @author Ethan Diosana
 *
 * */

#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// MY LIBRARIES
#include "ipTools.h"
#include "server_manager/serverManagerArguments.h"
#include "server_manager/serverManagerTools.h"

// MESSAGES
#define MSG_SERVER_CONNECT_ATTEMPT "[ATTEMPT]: attempting to connect to server...\n"
#define MSG_SERVER_CONNECT_SUCCESS "[SUCCESS]: connected to server.\n"
#define MSG_SERVER_CONNECT_FAILURE "[FAILURE]: failed to connect to server\n"

#define MSG_SENDING_PASSCODE_ATTEMPT "[ATTEMPT]: sending passcode to server...\n"
#define MSG_SENDING_PASSCODE_SUCCESS "[SUCCESS]: passcode sent to server.\n"
#define MSG_SENDING_PASSCODE_FAILURE "[FAILURE] passcode failed to send.\n"

#define MSG_RECEIVE_MESSAGE_ATTEMPT "[ATTEMPT]: receiving message from server...\n"
#define MSG_RECEIVE_MESSAGE_SUCCESS "[SUCCESS]: message received from server.\n"
#define MSG_RECEIVE_MESSAGE_FAILURE "[FAILURE]: failed to receive message from server\n"

#define MSG_USER_INPUT_FAILURE "[FAILURE]: failed to get user input\n"

// DEFAULTS
#define DEFAULT_PORT 3000

/**
 * @brief Connects to a server with the given params.
 * @param arguments The arguments struct
 */
int connect_to_server(const struct server_manager_arguments *arguments, int server_fd);

/**
 * @brief Sends the passcode to the server.
 * @param server_fd The server fd.
 * @param passcode The passcode to send.
 */
int send_passcode(int server_fd, const char *passcode);

/**
 * @brief Receives the response from the server and displays it.
 * @param server_fd The server fd.
 */
int receive_and_display_response(int server_fd);

/**
 * @brief Receives input from the user
 */
int get_user_input(char *input);

/**
 * @brief Sets up the ctrl c signal
 */
static void setup_signal_handler(void);

/**
 * @brief Handles ctrl c signal from user
 */
static void sig_handler(int signum);

static volatile sig_atomic_t exit_flag = 0;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(const int argc, const char *argv[])
{
    struct server_manager_arguments *arguments;
    int                              server_fd;    // The socket for the server
    char                             input[UINT16_MAX];

    display_divider("SERVER MANAGER START");

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        display_divider("SERVER MANAGER END");
        return EXIT_FAILURE;
    }

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Attempt to connect to the server.
    if(connect_to_server(arguments, server_fd) == EXIT_FAILURE)
    {
        fprintf(stderr, MSG_SERVER_CONNECT_FAILURE);
        free_server_manager_arguments(arguments);
        display_divider("SERVER MANAGER END");
        return EXIT_FAILURE;
    }

    setup_signal_handler();
    while(!exit_flag)
    {
        if(get_user_input(input) == EXIT_FAILURE)
        {
            fprintf(stderr, MSG_USER_INPUT_FAILURE);
            free_server_manager_arguments(arguments);
            display_divider("SERVER MANAGER END");
            close(server_fd);
            return EXIT_FAILURE;
        }

        if(send_message(server_fd, input) == EXIT_FAILURE)
        {
            fprintf(stderr, MSG_SENDING_PASSCODE_FAILURE);
            free_server_manager_arguments(arguments);
            perror("send_message");
            close(server_fd);
            return EXIT_FAILURE;
        }

        if(receive_and_display_response(server_fd) == EXIT_FAILURE)
        {
            perror("receive_and_display_response");
            free_server_manager_arguments(arguments);
            close(server_fd);
            return EXIT_FAILURE;
        }
    }

    close(server_fd);
    free_server_manager_arguments(arguments);
    display_divider("SERVER MANAGER END");
    return EXIT_SUCCESS;
}

int connect_to_server(const struct server_manager_arguments *arguments, int server_fd)
{
    struct sockaddr_in server_address;

    // Print divider for debugging.
    display_divider("connect_to_server start");

    // Allocate memory for the server_address.
    memset(&server_address, 0, sizeof(server_fd));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, arguments->ip, &server_address.sin_addr) <= 0)
    {
        perror("ip conversion failed");
        close(server_fd);
        display_divider("connect_to_server end");
        return EXIT_FAILURE;
    }

    printf(MSG_SERVER_CONNECT_ATTEMPT);
    if(connect(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("connect");
        close(server_fd);
        display_divider("connect_to_server end");
        return EXIT_FAILURE;
    }
    printf(MSG_SERVER_CONNECT_SUCCESS);

    // When connected, immediately send the passcode.
    if(send_passcode(server_fd, arguments->passcode))
    {
        perror("send_passcode");
        close(server_fd);
        return EXIT_FAILURE;
    }

    display_divider("connect_to_server end");
    return EXIT_SUCCESS;
}

int send_passcode(int server_fd, const char *passcode)
{
    printf(MSG_SENDING_PASSCODE_ATTEMPT);
    if(send_message(server_fd, passcode) == EXIT_FAILURE)
    {
        fprintf(stderr, MSG_SENDING_PASSCODE_FAILURE);
        perror("send_message");
        close(server_fd);
        return EXIT_FAILURE;
    }
    printf(MSG_SENDING_PASSCODE_SUCCESS);

    // Await the response from the server.
    if(receive_and_display_response(server_fd) == EXIT_FAILURE)
    {
        perror("receive_and_display_response");
        close(server_fd);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int receive_and_display_response(int server_fd)
{
    struct common_message *message;

    // Attempt to receive the message.
    printf(MSG_RECEIVE_MESSAGE_ATTEMPT);
    message = receive_message(server_fd);
    if(message == NULL)
    {
        fprintf(stderr, MSG_RECEIVE_MESSAGE_FAILURE);
        return EXIT_FAILURE;
    }
    printf(MSG_RECEIVE_MESSAGE_SUCCESS);

    printf("Server: %s\n", message->content);

    // Free the message struct.
    free(message->content);
    free(message);
    return EXIT_SUCCESS;
}

int get_user_input(char *input)
{
    char input_buffer[UINT16_MAX];

    memset(input_buffer, 0, UINT16_MAX);

    if(fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
    {
        return EXIT_FAILURE;
    }

    strncpy(input, input_buffer, UINT16_MAX);

    return EXIT_SUCCESS;
}

static void setup_signal_handler(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

// Disable specific clang compiler warning related to macro expansion.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

    // Set the signal handler function for SIGTSTP (Ctrl+Z) to 'sigtstp_handler'.
    sa.sa_handler = sig_handler;

// Restore the previous Clang compiler warning settings.
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

    sigemptyset(&sa.sa_mask);    // Clear the sa_mask, which is used to block signals during the signal handler execution.
    sa.sa_flags = 0;             // Set sa_flags to 0, indicating no special flags for signal handling.

    // Register the signal handler configuration ('sa') for the SIGINT signal.
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void sig_handler(int signum)
{
    exit_flag = 1;
}

#pragma GCC diagnostic pop
