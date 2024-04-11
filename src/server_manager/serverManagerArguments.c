#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MY LIBRARIES
#include "ipTools.h"
#include "server_manager/serverManagerArguments.h"
#include "server_manager/serverManagerTools.h"

void print_server_manager_arguments(struct server_manager_arguments arguments)
{
    printf("struct server_manager_arguments {\n\tip: %s\n\tport: %hu\n\tpasscode: %s\n}", arguments.ip, arguments.port, arguments.passcode);
}

void free_server_manager_arguments(struct server_manager_arguments *arguments)
{

    printf("[freeing server manager arguments...]\n");
    free(arguments->ip);
    free(arguments->passcode);
    free(arguments);
    printf("[server manager arguments freed]\n");
}

struct server_manager_arguments *parse_args(int argc, const char *argv[])
{
    struct server_manager_arguments *arguments;
    uint16_t                         port;

    display_divider("parse_args start");

    if(argc != SERVER_MANAGER_NUM_ARGS)
    {
        display_usage();
        fprintf(stderr, "invalid number of arguments");
        return NULL;
    }
    printf("valid number of arguments...\n");

    // Allocate memory for the arguments struct.
    arguments = malloc(sizeof(struct server_manager_arguments));
    if(arguments == NULL)
    {
        fprintf(stderr, "arguments struct failed to allocate");
        return NULL;
    }

    printf("arguments struct allocated...\n");

    // TODO: use convert_port instead.
    port = PORT_MIN;
    // TODO: validate ip using ipTools.

    // Set the arguments.
    arguments->ip       = strndup(argv[1], strlen(argv[1]));
    arguments->port     = port;
    arguments->passcode = strndup(argv[3], strlen(argv[3]));

    printf("arguments struct params set...\n\n");

    print_server_manager_arguments(*arguments);

    display_divider("parse_args end");

    return arguments;
}
