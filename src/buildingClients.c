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
// #define LINE_LENGTH 1024

// ----- Function Headers -----

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

// Signal Handling Functions
static void setup_signal_handler(void);
static void sigtstp_handler(int signum);

static volatile sig_atomic_t sigtstp_flag = 0;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
#define NUM_CLIENTS 32
#define NUM_PAIR 2
#define BUFFER UINT16_MAX

// ----- Main Function -----

// check if defined amount of clients can connect and disconnect
int create_client_max(int argc, char *argv[])
{
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);
    for(int i = 0; i < NUM_CLIENTS; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);

    for(int i = 0; i < NUM_CLIENTS; i++)
    {
        socket_close(client_sockets[i]);
    }

    return EXIT_SUCCESS;
}

// check if users can see global messages
int check_client_chat_message(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    // write message from client1
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("hello");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "hello", contentSize);

    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }
    if(strstr(readMessage, "hello") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int check_format(int argc, char *argv[])
{
    //    ssize_t  bytes_sent;
    uint8_t  version;
    uint16_t contentSize;
    char     content[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_socket;
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;
    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // connect client to server
    client_socket = socket_create(addr.ss_family, SOCK_STREAM, 0);
    socket_connect(client_socket, &addr, port);

    // check for message

    read(client_socket, &version, sizeof(uint8_t));
    if(version != 1)
    {
        perror("Version numbers do not match");
        return -1;
    }
    read(client_socket, &contentSize, sizeof(uint16_t));
    contentSize = ntohs(contentSize);
    if(contentSize == 0 || contentSize >= UINT16_MAX)
    {
        perror("Size is not within bounds");
        return -1;
    }
    read(client_socket, &content, contentSize);
    printf("\nPlease check if this is the expected string, starting strings should be a welcome message:\n%s\n\n", content);
    return EXIT_SUCCESS;
}

// check if user changes their username with command
int check_username_command(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    // write message from client1
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/u checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "/u checkusout", contentSize);

    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("hello");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "hello", contentSize);

    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "checkusout") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// check if user can get list of users online
int check_userlist_command(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;

    char                    readMessage[BUFFER];
    char                    readMessageTokenize[BUFFER];
    const char             *token;
    char                   *ptr;
    int                     count;
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;
    count      = 0;
    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < BASE_TEN; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[0], &read_version, sizeof(uint8_t));
    read(client_sockets[0], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[0], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[0], &read_version, sizeof(uint8_t));
    read(client_sockets[0], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[0], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/ul");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "/ul", contentSize);

    // client2 read message from client1
    read(client_sockets[0], &read_version, sizeof(uint8_t));
    read(client_sockets[0], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[0], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    strcpy(readMessageTokenize, readMessage);
    ptr   = NULL;
    token = strtok_r(readMessageTokenize, "\n", &ptr);
    do
    {
        if(token != NULL)
        {
            count++;
        }
        else
        {
            break;
        }
        token = strtok_r(NULL, "\n", &ptr);
    } while(token != NULL);

    if(count == BASE_TEN + 1)    //+1 is because of the header
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// check if user changes their username with command
int check_whisper_command(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    // write message from client1
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/w Client2 hi how's it going");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "/w Client2 hi how's it going", contentSize);

    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "Direct") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// check if user changes their username with command
int check_help_command(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_socket;
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    client_socket = socket_create(addr.ss_family, SOCK_STREAM, 0);
    socket_connect(client_socket, &addr, port);

    // read welcome msg
    read(client_socket, &read_version, sizeof(uint8_t));
    read(client_socket, &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_socket, readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_socket, &read_version, sizeof(uint8_t));
    read(client_socket, &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_socket, readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    // write message from client1
    write(client_socket, &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/h");
    converted_contentSize = htons(contentSize);
    write(client_socket, &converted_contentSize, sizeof(uint16_t));
    write(client_socket, "/h", contentSize);

    // client2 read message from client1
    read(client_socket, &read_version, sizeof(uint8_t));
    read(client_socket, &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_socket, readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    socket_close(client_socket);

    if(strstr(readMessage, "COMMAND LIST") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// check if user changes their username with command
int check_username_already_exists(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    // client1 sets username
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/u checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "/u checkusout", contentSize);

    // client2 attempts same username
    write(client_sockets[1], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("/u checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[1], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[1], "/u checkusout", contentSize);

    // client2 read message from server
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }
    // may change value if servers implement error code
    if(strstr(readMessage, "taken") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// checks if server can hold max version size
int check_version_max(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = UINT8_MAX;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1 with max version number
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "checkusout", contentSize);

    //    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "checkusout") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// checks if server can hold max version size
int check_version_min(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];

    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1 with max version number
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "checkusout", contentSize);

    //    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "checkusout") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// checks if server can hold max content size
int check_content_size_max(int argc, char *argv[])
{
    uint8_t  version;
    uint16_t contentSize;
    uint16_t converted_contentSize;
    uint8_t  read_version;
    uint16_t read_contentSize;
    char     readMessage[BUFFER];
    // 2730 characters = 21840 bits x 3 = 65520 near max
    const char *content =
        "rjhxkdoiryhtcixgdlymibjrokascocefagtrzwfkrrirgahkmmngavkttklgyflevjuaynphxhfhptlohgqegakysbsgkwajcxpmppbbbefdsnzaqgefdpvhllfumnxzppctgxolvmxinpbsqegldetzvivrilvgxbpfekqlpakguxlaxpgfnfljkunrvsklclmzbdcobpjwnririxilqkmhtoygmjetwzskvfawcjmlmypvieuoaljpdwyauntdkhnjzeqtslgskitffqikxnkudqfvneedtulfpbdoxbxozjnokwunszawlaqnlrgpkjiiznutslzdrtjfypcykqwterxuxdvjxsyvaptspcxklgvallalizcutdbavfhjfgcmwkwxxzcxypstxbdtpkkraehgtaajaqxghhyhwdvixggljisbmnzkwcpgdwnwkerfrjujdgamwofyhppkxyrtiyvzvgkyougzcpfunwghvcmygcgsqwiwogkohxpmpuwriyvyewmxowjrtkyzmejwsoicppxcrrhxpemdqdlirpuyincpqanrzuoamlblzkxhaltiqmskmkptoeixddfgprfrtjdosserjyczpedjbzivmsglgixysvgqmrsmqznrenierejompvdkxnwmkmdbrjedyhtpxufncdjpbeyzyxvbvecsrgxtkowaldtgdnqcpzvzluexwzcjyamtvbutikfheouoiiezxcihvvaupnabvnseftgphfpcqabyngnbkijnxzaubrauhjcpsmcdbuqodfwxykqnqmserjfbmfckonmyqlutctbhbtzqfxowvvfandaxbhpahofxsbguqizmefgzrsnuupuxkdobohjldaxjkkinfxgkylsmdyyyttmoztbonrxwymazhijsvesaepmfnimvoqcmgwfujfiiaqgaqmeeuhrrdoobzcaywbspmmuquawsbatzsfnlipqkcqvzvwwudoomzowebnxxquncbsitcoqhxhdksuijajopmffkiletopluzkewaexnckrbcyhlpkftatzoqhtnjgvntvtvddjoyswcbazxtwyhbboljmtgubnhjcelaregythowmkxcbvhktdejjfnthfozrjntpnfquowpjjdvdartowuhcfmdcpetaeqgkgigvpptqvwirtmfkzaeqydixvglcxvkfzkcvxvsiwyohyclcbjihqmdocxvxwiwraskqefkmjaqhawxnbntujwtassyyauriexxnwhezvxxwssrvxzbpzpohbwsxmswspxqizfjegwdhjdxnsgbaglfhzzvufietahydrnbbwpqfworlhtylwycivhvdbgiuteymejbksurhyksrghwgvpmxlsuesxltycquskjnopjaymufyvfqpwopzwcntcjvsgkblkhmaesincrhpqctpktstyayutncdzjxymbemnvmpuwefupmodpkxigifhpdllwnwjmxdydhcoudkdagpoffoiefbjuzkpkicbbtwneswwjgdjpymiqcomvhgmqypbmtrcjbbmrvzvawyaebatimwjwwalyfkhwhhznmsvxnifqzkjuvauhgltpkvvkuryseqjkqrstupnmzodzvoohoklhgbdlsfjxftwoqcogqxnkhbbbhsgqzqmemlhjeidgahtnwgxdvztqzniubgjknqivlnxkpdjxhezjkdevxsiseuvmfpazshlawgndsrvscjodfcwrbhawjijsmmsabgnsmpyxjmdgvamzwuqtvhydujkrauvzfewqpsrxvcwdneiuywetxujhnbnippnavgumkaaksbgkndzpifqitcnpjhqimpbxyjwvkccvyssmhcuavkjjeiymetdzzrnajgewfbveytfsvjrwvulbogjcjmaofqiyphgivfoqsmrcmqhzfclauxsjvqwugkymtbnhzqcskuaiyanjkqmwoavmvjigkgzhfpcoaeagjukgvcxavqgizbdirbmokpebjgfwedbewqhrwmuqwgchhfthtuqpikotxhpbodaepfteqjihwggqtpuptzbdkmrbwausvskzhozfbuiaioucslvpypapfujgjpthlslttfyixqjxwfrqsyylsielqinqjbysdvdlzmspltkpglnxvgwxsmheqqvvqjwrpvitexkukivkukmxydbqpqrylfgrekxpqjwihhfvtjhccfxerjnrrphiyplbjxqodjywebjhqimzjomfdiqwqgxyoggcmxicsztqsqaxhevzfxtkflweppoxfougyyfymfygtstvauyqadwqlhwmvwykthlahdxdyxwnafrknefyvldetzuwbspfrprrumlnhloozqunqudfagokovjnvkbvceuafuivsikbdsduseegbmgtfhsclshlshokdxjcjaqoigziuvzfdehaybfavjaborscqdtywckvsbkajxxiydwdowjgvzeaqhuorbtilsnvvzkcgxahkujuavalqlquqfnpypnpzljqlxjjkeqetjftbxzmbbeebnpebwqrojftnpfgforcbixmunleyinxkytblugdgdyvrnsdxrgoljcrumazdxwjycegyfaqxigliyjnkxkfamirrovpmvzcbwdhvsycrmmknzyyuwrcccwntlcjomruaxnrwrfdglavbuarrpqhrysmbbz";
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1 with max content size
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = UINT16_MAX;
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    // should add up to UINT16_MAX
    for(int i = 0; i < (NUM_PAIR + 1); i++)
    {
        write(client_sockets[0], content, strlen(content));
    }
    write(client_sockets[0], "a", strlen("a"));

    //    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "rjhxkdoiryhtcixgdlym") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// checks if server can hold max version size
int check_content_size_min(int argc, char *argv[])
{
    uint8_t                 version;
    uint16_t                contentSize;
    uint16_t                converted_contentSize;
    uint8_t                 read_version;
    uint16_t                read_contentSize;
    char                    readMessage[BUFFER];
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = 1;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1 with max content size
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("i");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    // should add up to UINT16_MAX
    write(client_sockets[0], "i", contentSize);

    //    // client2 read message from client1
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "i") != NULL)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// checks if server can hold max version size
int check_version_num(int argc, char *argv[])
{
    uint8_t                 version;
    uint16_t                contentSize;
    uint16_t                converted_contentSize;
    uint8_t                 read_version;
    uint16_t                read_contentSize;
    char                    readMessage[BUFFER];
    ssize_t                 bytes_read;
    char                   *ip_address;
    char                   *port_str;
    in_port_t               port;
    int                     client_sockets[NUM_CLIENTS];
    struct sockaddr_storage addr;

    ip_address = NULL;
    port_str   = NULL;
    version    = NUM_CLIENTS;

    parse_arguments(argc, argv, &ip_address, &port_str);
    handle_arguments(argv[0], ip_address, port_str, &port);
    convert_address(ip_address, &addr);

    // create two clients, one to input username, the other to check
    for(int i = 0; i < NUM_PAIR; i++)
    {
        client_sockets[i] = socket_create(addr.ss_family, SOCK_STREAM, 0);
        socket_connect(client_sockets[i], &addr, port);
    }
    sleep(1);    // allow connections to settle

    // read welcome msg from client2
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }
    read(client_sockets[1], &read_version, sizeof(uint8_t));
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
                                                 //        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        return -1;
    }

    // write message from client1 with max version number
    write(client_sockets[0], &version, sizeof(uint8_t));
    contentSize           = (uint16_t)strlen("checkusout");
    converted_contentSize = htons(contentSize);
    write(client_sockets[0], &converted_contentSize, sizeof(uint16_t));
    write(client_sockets[0], "checkusout", contentSize);

    //    // client2 read message from client1
    bytes_read = read(client_sockets[1], &read_version, sizeof(uint8_t));
    if(bytes_read <= 0)
    {
        return 0;
    }
    if(read_version != version)
    {
        return -1;
    }
    read(client_sockets[1], &read_contentSize, sizeof(uint16_t));
    read_contentSize = ntohs(read_contentSize);
    if(read_contentSize < BUFFER)
    {
        read(client_sockets[1], readMessage, read_contentSize);
        readMessage[read_contentSize] = '\0';    // Null-terminate the received message
        printf("\nRead version: %u\nContent Size: %u\nString: %s\n", read_version, read_contentSize, readMessage);
    }
    else
    {
        printf("read failed");
        return -1;
    }

    // close all sockets
    for(int i = 0; i < NUM_PAIR; i++)
    {
        socket_close(client_sockets[i]);
    }

    if(strstr(readMessage, "checkusout") != NULL)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// ----- Function Definitions -----

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

// Signal Handling Functions

/**
 * Sets up a signal handler for the application.
 */
static void setup_signal_handler(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

// Disable specific clang compiler warning related to macro expansion.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

    // Set the signal handler function for SIGTSTP (Ctrl+Z) to 'sigtstp_handler'.
    sa.sa_handler = sigtstp_handler;

// Restore the previous Clang compiler warning settings.
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

    sigemptyset(&sa.sa_mask);    // Clear the sa_mask, which is used to block signals during the signal handler execution.
    sa.sa_flags = 0;             // Set sa_flags to 0, indicating no special flags for signal handling.

    // Register the signal handler configuration ('sa') for the SIGINT signal.
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * Signal handler function for the SIGTSTP (Ctrl+Z) signal.
 * @param signum the signal number, typically SIGTSTP (2) in this context
 */
static void sigtstp_handler(int signum)
{
    sigtstp_flag = 1;
}

#pragma GCC diagnostic pop
