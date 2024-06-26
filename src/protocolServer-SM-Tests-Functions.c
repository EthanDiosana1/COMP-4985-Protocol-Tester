#include "protocolServer-SM-Tests-Functions.h"
#include "server_manager/serverManagerFunctions.h"
#include "server_manager/serverManagerTools.h"

// Data Types and Limits
#include <inttypes.h>
#include <stdint.h>

// Error Handling
#include <errno.h>

// Network Programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Signal Handling
#include <signal.h>

// Standard Library
#include "../include/buildingClients.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Macros
#define UNKNOWN_OPTION_MESSAGE_LEN 24
#define BASE_TEN 10

#define CORRECT_PASSCODE "hellyabrother\0"
#define CORRECT_PASSCODE_LENGTH 14
#define INCORRECT_PASSCODE "Aa\0"
#define INCORRECT_PASSCODE_LENGTH 3
#define ARGV_PASSCODE_INDEX 3
#define NUM_CLIENT_ARGS 3
#define STRING_TO_SEND "HELLO WORLD\0"
#define CLOSE_SERVER_COMMAND "/q\0"

// Argument Parsing
static void      parse_arguments(int argc, char *argv[], char **ip_address, char **port);
static void      handle_arguments(const char *binary_name, const char *ip_address, const char *port_str, in_port_t *port);
static in_port_t parse_in_port_t(const char *binary_name, const char *port_str);

// Error Handling
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);

// Network Handling
static void convert_address(const char *address, struct sockaddr_storage *addr);
static int  socket_create(int domain, int type, int protocol);
static void socket_connect(int sockfd, struct sockaddr_storage *addr, in_port_t port);
static void socket_close(int sockfd);

int connect_correct_passcode(int argc, char *argv[])
{
    int                              server_fd;    // The socket for the server
    struct server_manager_arguments *arguments;
    const char                       correctPasscode[CORRECT_PASSCODE_LENGTH] = CORRECT_PASSCODE;
    argv[ARGV_PASSCODE_INDEX]                                                 = strdup(correctPasscode);

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        return EXIT_FAILURE;
    }

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Attempt to connect to the server.
    if(connect_to_server(arguments, server_fd) == EXIT_FAILURE)
    {
        fprintf(stderr, "Failed to connect to server.\n");
        perror("connect_to_server");
        close(server_fd);
        free_server_manager_arguments(arguments);
        return EXIT_FAILURE;
    }

    close(server_fd);
    free_server_manager_arguments(arguments);
    return EXIT_SUCCESS;
}

int connect_incorrect_passcode(int argc, char *argv[])
{
    int                              server_fd;    // The socket for the server
    struct server_manager_arguments *arguments;
    const char                       incorrectPasscode[INCORRECT_PASSCODE_LENGTH] = INCORRECT_PASSCODE;
    argv[ARGV_PASSCODE_INDEX]                                                     = strdup(incorrectPasscode);

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        return EXIT_FAILURE;
    }

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Attempt to connect to the server.
    if(connect_to_server(arguments, server_fd) == EXIT_FAILURE)
    {
        fprintf(stderr, "Failed to connect to server.\n");
        perror("connect_to_server");
        close(server_fd);
        free_server_manager_arguments(arguments);
        return EXIT_FAILURE;
    }

    close(server_fd);
    free_server_manager_arguments(arguments);
    return EXIT_SUCCESS;
}

int client_connect_normal(int argc, char *argv[])
{
    ssize_t                          clientConnectResult = EXIT_FAILURE;
    char                            *ip_address;
    char                            *port_str;
    in_port_t                        port;
    struct sockaddr_storage          addr;
    char                            *buffer;
    int                              server_fd;    // The socket for the server
    int                              client_socket;
    struct server_manager_arguments *arguments;
    const char                       correctPasscode[CORRECT_PASSCODE_LENGTH] = CORRECT_PASSCODE;
    argv[ARGV_PASSCODE_INDEX]                                                 = strdup(correctPasscode);

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        return EXIT_FAILURE;
    }

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Attempt to connect to the server.
    if(connect_to_server(arguments, server_fd) == EXIT_FAILURE)
    {
        fprintf(stderr, "Failed to connect to server.\n");
        perror("connect_to_server");
        close(server_fd);
        free_server_manager_arguments(arguments);
        return EXIT_FAILURE;
    }
    free_server_manager_arguments(arguments);

    // Fix args for client.
    argc = NUM_CLIENT_ARGS;

    printf("client starting...\n");
    // Spawn a client and have it connect to the server.
    ip_address = NULL;
    port_str   = NULL;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);
    client_socket = socket_create(addr.ss_family, SOCK_STREAM, 0);

    printf("client connecting to server...\n");
    socket_connect(client_socket, &addr, port);

    // Attempt to write to the client.
    buffer              = strdup(STRING_TO_SEND);
    clientConnectResult = write(client_socket, &buffer, sizeof(buffer));

    socket_close(client_socket);

    // Close the server manager.
    close(server_fd);
    free(buffer);
    return (int)clientConnectResult > -1;
}

int client_connect_after_sending_q(int argc, char *argv[])
{
    ssize_t                          clientConnectResult = EXIT_FAILURE;
    char                            *ip_address;
    char                            *port_str;
    in_port_t                        port;
    struct sockaddr_storage          addr;
    char                            *buffer;
    int                              server_fd;    // The socket for the server
    int                              client_socket;
    struct server_manager_arguments *arguments;
    const char                       correctPasscode[CORRECT_PASSCODE_LENGTH] = CORRECT_PASSCODE;
    argv[ARGV_PASSCODE_INDEX]                                                 = strdup(correctPasscode);

    arguments = parse_args(argc, argv);

    if(arguments == NULL)
    {
        fprintf(stderr, "failed to set arguments\n");
        return EXIT_FAILURE;
    }

    // Create the server socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Attempt to connect to the server.
    if(connect_to_server(arguments, server_fd) == EXIT_FAILURE)
    {
        fprintf(stderr, "Failed to connect to server.\n");
        perror("connect_to_server");
        close(server_fd);
        free_server_manager_arguments(arguments);
        return EXIT_FAILURE;
    }
    free_server_manager_arguments(arguments);

    // Send /q to the server.
    send_message(server_fd, CLOSE_SERVER_COMMAND);

    // Fix args for client.
    argc = NUM_CLIENT_ARGS;

    printf("client starting...\n");
    // Spawn a client and have it connect to the server.
    ip_address = NULL;
    port_str   = NULL;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);
    client_socket = socket_create(addr.ss_family, SOCK_STREAM, 0);

    printf("client connecting to server...\n");
    socket_connect(client_socket, &addr, port);

    // Attempt to write to the client.
    buffer              = strdup(STRING_TO_SEND);
    clientConnectResult = write(client_socket, &buffer, sizeof(buffer));

    socket_close(client_socket);

    // Close the server manager.
    close(server_fd);
    free(buffer);
    if((int)clientConnectResult > -1)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// Argument Parsing Functions
static void parse_arguments(const int argc, char *argv[], char **ip_address, char **port)
{
    int opt;
    opterr = 0;

    // Option parsing
    while((opt = getopt(argc, argv, "h")) != -1)
    {
        switch(opt)
        {
            case 'h':    // Help argument
            {
                usage(argv[0], EXIT_SUCCESS, NULL);
            }
            case '?':    // Unknown argument
            {
                char message[UNKNOWN_OPTION_MESSAGE_LEN];

                snprintf(message, sizeof(message), "Unknown option '-%c'.", optopt);
                usage(argv[0], EXIT_FAILURE, message);
            }
            default:
            {
                usage(argv[0], EXIT_FAILURE, NULL);
            }
        }
    }

    if(optind >= argc)    // Check for sufficient args
    {
        usage(argv[0], EXIT_FAILURE, "The ip address and port are required.");
    }

    if(optind + 1 >= argc)    // Check for port arg
    {
        usage(argv[0], EXIT_FAILURE, "The port is required.");
    }

    if(optind < argc - 2)    // Check for extra args
    {
        usage(argv[0], EXIT_FAILURE, "Error: Too many arguments.");
    }

    *ip_address = argv[optind];
    *port       = argv[optind + 1];
}

static void handle_arguments(const char *binary_name, const char *ip_address, const char *port_str, in_port_t *port)
{
    if(ip_address == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "The ip address is required.");
    }

    if(port_str == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "The port is required.");
    }

    *port = parse_in_port_t(binary_name, port_str);
}

static in_port_t parse_in_port_t(const char *binary_name, const char *port_str)
{
    char     *endptr;
    uintmax_t parsed_value;

    errno        = 0;
    parsed_value = strtoumax(port_str, &endptr, BASE_TEN);

    // Check for errno was signalled
    if(errno != 0)
    {
        perror("Error parsing in_port_t.");
        exit(EXIT_FAILURE);
    }

    // Check for any non-numeric characters in the input string
    if(*endptr != '\0')
    {
        usage(binary_name, EXIT_FAILURE, "Invalid characters in input.");
    }

    // Check if the parsed value is within valid range of in_port_t
    if(parsed_value > UINT16_MAX)
    {
        usage(binary_name, EXIT_FAILURE, "in_port_t value out of range.");
    }

    return (in_port_t)parsed_value;
}

// Error Handling Functions

_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s [-h] <ip address> <port>\n", program_name);
    fputs("Options:\n", stderr);
    fputs(" -h Display this help message\n", stderr);
    exit(exit_code);
}

// Network Handling Functions

/**
 * Converts the address from a human-readable string into a binary representation.
 * @param address string IP address in human-readable format (e.g., "192.168.0.1")
 * @param addr    pointer to the struct sockaddr_storage where the binary representation will be stored
 */
static void convert_address(const char *address, struct sockaddr_storage *addr)
{
    memset(addr, 0, sizeof(*addr));

    // Converts the str address to binary address and checks for IPv4 or IPv6
    if(inet_pton(AF_INET, address, &(((struct sockaddr_in *)addr)->sin_addr)) == 1)
    {
        // IPv4 address
        addr->ss_family = AF_INET;
    }
    else if(inet_pton(AF_INET6, address, &(((struct sockaddr_in6 *)addr)->sin6_addr)) == 1)
    {
        // IPv6 address
        addr->ss_family = AF_INET6;
    }
    else
    {
        fprintf(stderr, "%s is not an IPv4 or IPv6 address\n", address);
        exit(EXIT_FAILURE);
    }
}

/**
 * Creates a socket with the specified domain, type, and protocol.
 * @param domain   the communication domain, e.g., AF_INET for IPv4 or AF_INET6 for IPv6
 * @param type     the socket type, e.g., SOCK_STREAM for a TCP socket or SOCK_DGRAM for a UDP socket
 * @param protocol the specific protocol to be used, often set to 0 for the default protocol
 * @return         the file descriptor for the created socket, or -1 on error
 */
static int socket_create(int domain, int type, int protocol)
{
    int sockfd;

    sockfd = socket(domain, type, protocol);

    if(sockfd == -1)
    {
        fprintf(stderr, "Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Establishes a network connection to a remote address and port using a given socket.
 * @param sockfd the file descriptor of the socket for the connection
 * @param addr   a pointer to a struct sockaddr_storage containing the remote address
 * @param port   the port to which the connection should be established
 */
static void socket_connect(int sockfd, struct sockaddr_storage *addr, in_port_t port)
{
    char      addr_str[INET6_ADDRSTRLEN];    // Array to store human-readable IP address for either IPv4 or IPv6
    in_port_t net_port;                      // Stores network byte order representation of port number
    socklen_t addr_len;                      // Stores the address length

    // Converts binary IP address (IPv4 or IPv6) to a human-readable string, stores it in addr_str, and handles errors
    if(inet_ntop(addr->ss_family, addr->ss_family == AF_INET ? (void *)&(((struct sockaddr_in *)addr)->sin_addr) : (void *)&(((struct sockaddr_in6 *)addr)->sin6_addr), addr_str, sizeof(addr_str)) == NULL)
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    //    printf("Connecting to %s:%u\n", addr_str, port);
    net_port = htons(port);    // Convert port number to network byte order (big endian)

    // Handle IPv4
    if(addr->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4_addr;

        ipv4_addr           = (struct sockaddr_in *)addr;
        ipv4_addr->sin_port = net_port;
        addr_len            = sizeof(struct sockaddr_in);
    }
    // Handle IPv6
    else if(addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6_addr;

        ipv6_addr            = (struct sockaddr_in6 *)addr;
        ipv6_addr->sin6_port = net_port;
        addr_len             = sizeof(struct sockaddr_in6);
    }
    else
    {
        fprintf(stderr, "Internal error: addr->ss_family must be AF_INET or AF_INET6, was: %d\n", addr->ss_family);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if(connect(sockfd, (struct sockaddr *)addr, addr_len) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    //    printf("Connected to: %s:%u\n", addr_str, port);
}

/**
 * Closes a socket with the specified file descriptor.
 * @param sockfd the file descriptor of the socket to be closed
 */
static void socket_close(int sockfd)
{
    if(close(sockfd) == -1)
    {
        perror("error closing socket");
        exit(EXIT_FAILURE);
    }
}
