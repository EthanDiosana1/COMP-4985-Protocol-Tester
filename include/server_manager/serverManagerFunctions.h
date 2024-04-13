#ifndef SERVER_MANAGER_FUNCTIONS_C
#define SERVER_MANAGER_FUNCTIONS_C

#include "server_manager/serverManagerArguments.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Runs the server manager and connects it to the server using the params.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @returns int The exit code of the server manager.
 */
int run_server_manager(int argc, char *argv[]);

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

#endif
