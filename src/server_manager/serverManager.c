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

/**
 * @brief Connects to a server with the given params.
 * @param arguments The arguments struct
 */
int connect_to_server(const struct server_manager_arguments *arguments);

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

    free_server_manager_arguments(arguments);

    display_divider("SERVER MANAGER END");
    return EXIT_SUCCESS;
}

int connect_to_server(const struct server_manager_arguments *arguments)
{
    int                server_fd;    // The socket for the server
    struct sockaddr_in server_address;

    // Print divider for debugging.
    display_divider("CONNECT TO SERVER START");
    print_server_manager_arguments(*arguments);

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Allocate memory for the server_address.
    memset(&server_address, 0, sizeof(server_fd));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = arguments->port;

    if(connect(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("connect");
        close(server_fd);
        return EXIT_FAILURE;
    }

    display_divider("CONNECT TO SERVER END");
    return EXIT_SUCCESS;
}
