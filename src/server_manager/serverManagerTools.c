#include "server_manager/serverManagerTools.h"
#include <stdio.h>
#include <stdlib.h>

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

int send_message(int server_fd, struct common_message message)
{
    // Send the version
    if(send(server_fd, &message.version, sizeof(message.version), 0) != sizeof(message.version))
    {
        perror("send version failed");
        return EXIT_FAILURE;
    }

    // Send the size
    if(send(server_fd, &message.size, sizeof(message.size), 0) < 0)
    {
        perror("send size failed");
        return EXIT_FAILURE;
    }

    // Send the content
    if(send(server_fd, &message.content, message.size, 0) < 0)
    {
        perror("send content failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
