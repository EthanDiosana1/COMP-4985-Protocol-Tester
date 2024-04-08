#include "server_manager/serverManagerTools.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_RECEIVE_CONTENT_FAILURE "[FAILURE]: failed to receive content from server\n"

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
    printf("\n%-10s %-25s %s\n\n", divider, text, divider);
}

int send_message(int server_fd, const char *content)
{
    struct common_message message;

    message.version = MESSAGE_PROTOCOL_VERSION;
    message.size    = (uint16_t)strlen(content);
    message.content = strdup(content);

    // Send the version
    if(send(server_fd, &message.version, sizeof(message.version), 0) != sizeof(message.version))
    {
        perror("send version failed");
        free(message.content);
        return EXIT_FAILURE;
    }

    // Send the size
    if(send(server_fd, &message.size, sizeof(message.size), 0) < 0)
    {
        perror("send size failed");
        free(message.content);
        return EXIT_FAILURE;
    }

    // Send the content
    if(send(server_fd, &message.content, message.size, 0) < 0)
    {
        perror("send content failed");
        free(message.content);
        return EXIT_FAILURE;
    }

    free(message.content);

    return EXIT_SUCCESS;
}

struct common_message *receive_message(int server_fd)
{
    struct common_message *message = malloc(sizeof(struct common_message));
    ssize_t                bytes_received;
    if(!message)
    {
        perror("malloc failed");
        return NULL;
    }

    // Receive the version
    bytes_received = recv(server_fd, &message->version, (ssize_t)sizeof(message->version), 0);
    if(bytes_received != (ssize_t)sizeof(message->version))
    {
        perror("receive version failed");
        free(message);
        return NULL;
    }

    // Check for correct protocol version
    if(message->version != MESSAGE_PROTOCOL_VERSION)
    {
        fprintf(stderr, "Received message with incorrect version number\n");
        free(message);
        return NULL;
    }

    // Receive the size
    bytes_received = recv(server_fd, &message->size, sizeof(message->size), 0);
    if(bytes_received != sizeof(message->size))
    {
        printf("message->size: %hu\n", message->size);
        perror("receive size failed");
        free(message);
        return NULL;
    }

    // Correct network order.
    message->size = ntohs(message->size);

    // Allocate memory for the content
    message->content = malloc(sizeof(char) * (message->size + 1));
    // +1 for null terminator

    if(!message->content)
    {
        perror("malloc failed");
        free(message);
        return NULL;
    }

    message->content[message->size] = '\0';

    // Receive the content
    bytes_received = recv(server_fd, message->content, message->size, 0);
    if(bytes_received != (message->size * sizeof(char)))
    {
        fprintf(stderr, MSG_RECEIVE_CONTENT_FAILURE);
        printf("message->size %hu\n", message->size);
        printf("bytes_received: %zd\n", bytes_received);
        printf("message->content: %s\n", message->content);
        perror("receive content failed");
        free(message->content);
        free(message);
        return NULL;
    }

    message->content[message->size] = '\0';    // Null-terminate the content

    return message;
}
