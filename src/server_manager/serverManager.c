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

#define MSG_SENDING_COMMAND_ATTEMPT "[ATTEMPT]: sending command to server...\n"
#define MSG_SENDING_COMMAND_SUCCESS "[SUCCESS]: command sent to server.\n"
#define MSG_SENDING_COMMAND_FAILURE "[FAILURE]: command failed to send\n"

/**
 * @brief Connects to a server with the given params.
 * @param arguments The arguments struct
 */
int connect_to_server(const struct server_manager_arguments *arguments);

/**
 * @brief Sends the passcode to the server.
 */
int send_passcode(int server_fd, const char *passcode);

/**
 * @brief Sends a command to the server.
 */
int send_command(int server_fd, const char *command);

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
    server_address.sin_port   = arguments->port;

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

    display_divider("connect_to_server end");
    return EXIT_SUCCESS;
}

int send_passcode(int server_fd, const char *passcode)
{
    struct common_message message;

    if(passcode == NULL)
    {
        perror("send message");
        return EXIT_FAILURE;
    }

    message.version = MESSAGE_PROTOCOL_VERSION;
    message.size    = (uint16_t)strlen(passcode);
    message.content = strdup(passcode);

    if(message.content == NULL)
    {
        perror("send message");
        printf(MSG_SENDING_PASSCODE_FAILURE);
        return EXIT_FAILURE;
    }

    // Attempt to send the message to the server.
    printf(MSG_SENDING_PASSCODE_ATTEMPT);
    if(send_message(server_fd, message) == EXIT_FAILURE)
    {
        perror("send message");
        printf(MSG_SENDING_PASSCODE_FAILURE);
        return EXIT_FAILURE;
    }

    free(message.content);
    printf(MSG_SENDING_PASSCODE_SUCCESS);

    return EXIT_SUCCESS;
}

int send_command(int server_fd, const char *command)
{
    struct common_message message;

    if(command == NULL)
    {
        perror("send_command");
        return EXIT_FAILURE;
    }

    message.version = MESSAGE_PROTOCOL_VERSION;
    message.size    = (uint16_t)strlen(command);
    message.content = strdup(command);

    if(message.content == NULL)
    {
        perror("send message");
        printf(MSG_SENDING_COMMAND_FAILURE);
        return EXIT_FAILURE;
    }

    // Attempt to send the message to the server.
    printf(MSG_SENDING_COMMAND_ATTEMPT);
    if(send_message(server_fd, message) == EXIT_FAILURE)
    {
        perror("send message");
        printf(MSG_SENDING_COMMAND_FAILURE);
        return EXIT_FAILURE;
    }

    free(message.content);
    printf(MSG_SENDING_COMMAND_SUCCESS);

    return EXIT_SUCCESS;
}
