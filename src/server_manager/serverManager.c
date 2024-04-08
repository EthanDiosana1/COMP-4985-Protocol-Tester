/** This file runs a server manager.
 *
 * It is used to test how a server
 * handles server managers.
 *
 * @author Ethan Diosana
 *
 * */

#include <arpa/inet.h>
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

// DEFAULTS
#define DEFAULT_PORT 3000

/**
 * @brief Connects to a server with the given params.
 * @param arguments The arguments struct
 */
int connect_to_server(const struct server_manager_arguments *arguments);

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

int main(const int argc, const char *argv[])
{
    struct server_manager_arguments *arguments;

    display_divider("SERVER MANAGER START");

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        display_divider("SERVER MANAGER END");
        return EXIT_FAILURE;
    }

    // Attempt to connect to the server.
    if(connect_to_server(arguments) == EXIT_FAILURE)
    {
        fprintf(stderr, MSG_SERVER_CONNECT_FAILURE);
        free_server_manager_arguments(arguments);
        display_divider("SERVER MANAGER END");
        return EXIT_FAILURE;
    }

    free_server_manager_arguments(arguments);
    display_divider("SERVER MANAGER END");
    return EXIT_SUCCESS;
}

int connect_to_server(const struct server_manager_arguments *arguments)
{
    int                server_fd;    // The socket for the server
    struct sockaddr_in server_address;

    // Print divider for debugging.
    display_divider("connect_to_server start");

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

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

    close(server_fd);
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

    printf(MSG_RECEIVE_MESSAGE_ATTEMPT);
    message = receive_message(server_fd);
    if(message == NULL)
    {
        fprintf(stderr, MSG_RECEIVE_MESSAGE_FAILURE);
        return EXIT_FAILURE;
    }
    printf(MSG_RECEIVE_MESSAGE_SUCCESS);

    printf("Server: %s\n", message->content);

    free(message->content);
    free(message);
    return EXIT_SUCCESS;
}
