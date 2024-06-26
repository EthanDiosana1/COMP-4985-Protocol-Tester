#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MY LIBRARIES
#include "ipTools.h"
#include "server_manager/serverManagerArguments.h"
#include "server_manager/serverManagerTools.h"

// Error messages
#define ERR_INVALID_NUM_ARGS "invalid number of arguments\n"
#define ERR_INVALID_PORT "port must be between %d and %d inclusive. port value: %d\n"

// #define ERR_INVALID_IPV4 "invalid ipv4\n"

void print_server_manager_arguments(struct server_manager_arguments arguments)
{
    printf("struct server_manager_arguments {\n\tip: %s\n\tport: %hu\n\tpasscode: %s\n}\n", arguments.ip, arguments.port, arguments.passcode);
}

void free_server_manager_arguments(struct server_manager_arguments *arguments)
{
    printf("[freeing server manager arguments...]\n");
    free(arguments->ip);
    free(arguments->passcode);
    free(arguments);
    printf("[server manager arguments freed]\n");
}

struct server_manager_arguments *parse_args(int argc, char *argv[])
{
    struct server_manager_arguments *arguments;
    uint16_t                        *port;

    display_divider("parse_args start");

    if(argc != SERVER_MANAGER_NUM_ARGS)
    {
        display_usage();
        fprintf(stderr, ERR_INVALID_NUM_ARGS);
        return NULL;
    }
    printf("valid number of arguments...\n");

    // Validate ip using ipTools.
    printf("checking if ipv4 is valid...\n");
    printf("check not implemented, skipping\n");

    // Convert the string to a uint16_t.
    port = str_to_uint16_t(argv[2]);

    if(port == NULL)
    {
        fprintf(stderr, "port failed to convert\n");
        free(port);
        return NULL;
    }

    // Validate port.
    if(*port < PORT_MIN || *port > PORT_MAX)
    {
        fprintf(stderr, ERR_INVALID_PORT, PORT_MIN, PORT_MAX, *port);
        free(port);
        return NULL;
    }

    // Allocate memory for the arguments struct.
    arguments = malloc(sizeof(struct server_manager_arguments));
    if(arguments == NULL)
    {
        fprintf(stderr, "arguments struct failed to allocate\n");
        return NULL;
    }

    printf("arguments struct allocated...\n");

    // Set the arguments.
    arguments->ip   = strndup(argv[1], strlen(argv[1]));
    arguments->port = *port;
    free(port);    // Free old port pointer.
    arguments->passcode = strndup(argv[3], strlen(argv[3]));
    printf("arguments struct params set...\n\n");

    print_server_manager_arguments(*arguments);

    display_divider("parse_args end");

    return arguments;
}
