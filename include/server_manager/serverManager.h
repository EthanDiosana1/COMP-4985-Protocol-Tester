#ifndef SERVER_MANAGER_C
#define SERVER_MANAGER_C

#include "server_manager/serverManagerArguments.h"

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

int run_server_manager(int argc, const char *argv[]);

#endif
