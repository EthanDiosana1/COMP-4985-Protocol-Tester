#ifndef SERVER_MANAGER_TOOLS
#define SERVER_MANAGER_TOOLS

#include <stdint.h>
#include <sys/socket.h>

#define MESSAGE_PROTOCOL_VERSION 0

/** Prints a string that displays how to use this app. */
void display_usage(void);

/** Prints a string describing what this app is for. */
void display_purpose(void);

/**
 * @brief Displays a divider block.
 * @param text The text to display.
 */
void display_divider(const char *text);

/**
 * @brief Holds message information.
 */
struct common_message
{
    uint8_t  version;    // The protocol version.
    uint16_t size;       // The size of the message.
    char    *content;
};

/**
 * @brief Sends a message to the given server fd.
 *
 * @param server_fd The server to send to.
 * @param content The content to send.
 *
 * @returns int
 */
int send_message(int server_fd, const char *content);

/**
 * @brief Receives a message from the server.
 *
 * @param server_fd The server to receive from.
 */
struct common_message *receive_message(int server_fd);

#endif
