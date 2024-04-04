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

#define NUM_ARGS 4

struct arguments
{
    const char     *ip;
    const uint16_t *port;
    const char     *passcode;
};

/** Prints a string that displays how to use this app. */
void display_usage(void);

/** Prints a string describing what this app is for. */
void display_purpose(void);

/**
 * @brief Returns a arguments struct if the arguments are valid.
 *
 * @param argc The number of args.
 * @param argv The arguments.
 */
struct arguments *parse_args(int argc, const char *argv[]);

/**
 * @brief Connects to a server with the given params.
 *
 * @param ip The ip of the server to connect to
 * @param port The port of the server to connec to
 * @param passcode The passcode of the server to connect to.
 */
int connect_to_server(const char *ip, uint16_t port, const char *passcode);

/**
 * @brief Displays a divider block.
 * @param text The text to display.
 */
void display_divider(const char *text);

int main(const int argc, const char *argv[])
{
    struct arguments *arguments;

    arguments = parse_args(argc, argv);

    if(!arguments)
    {
        fprintf(stderr, "failed to set arguments");
        return EXIT_FAILURE;
    }

    display_divider("SERVER MANAGER START");

    // printf("Continuing with settings: \n\tip: %s\n\tport: %u\n\tpasscode: %s\n",
    //         arguments->ip, arguments->port, arguments->passcode);

    if(arguments)
    {
        free(arguments);
    }

    display_divider("SERVER MANAGER END");
    return EXIT_SUCCESS;
}

void display_usage(void)
{
    display_purpose();
    printf("\n");
    printf("Usage: ./serverManager <ip> <port> <passcode>\n");
}

void display_purpose(void)
{
    printf("Purpose: This app is meant to simulate a server manager for the purpose of testing servers.\n");
}

void display_divider(const char *text)
{
    const char *divider = "------";
    printf("\n%s %s %s\n\n", divider, text, divider);
}

struct arguments *parse_args(int argc, const char *argv[])
{
    struct arguments *arguments;

    if(argc != NUM_ARGS)
    {
        display_usage();
        perror("invalid number of arguments");
        return NULL;
    }

    // Convert the port from char to uint16_t.

    arguments = malloc(sizeof(arguments));
    if(!arguments)
    {
        fprintf(stderr, "arguments struct failed to allocate");
        return NULL;
    }

    arguments->ip       = strndup(argv[1], strlen(argv[1]));
    arguments->port     = strndup(argv[2], strlen(argv[2]));
    arguments->passcode = strndup(argv[3], strlen(argv[3]))

        return arguments;
}

int connect_to_server(const char *ip, uint16_t port, const char *passcode)
{
    // Initialize the server.
    return EXIT_SUCCESS;
}
