/** This file runs a server manager.
 *
 * It is used to test how a server
 * handles server managers.
 *
 * @author Ethan Diosana
 *
 * */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MY LIBRARIES
#include "ipTools.h"
#include "serverManagerArguments.h"
#include "serverManagerTools.h"

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
        fprintf(stderr, "failed to set arguments");
        return EXIT_FAILURE;
    }

    free_server_manager_arguments(arguments);

    display_divider("SERVER MANAGER END");
    return EXIT_SUCCESS;
}

int connect_to_server(const struct server_manager_arguments *arguments)
{
    display_divider("CONNECT TO SERVER START");
    // Initialize the server.

    print_server_manager_arguments(*arguments);

    display_divider("CONNECT TO SERVER END");
    return EXIT_SUCCESS;
}
