#include "server_manager/serverManagerTools.h"
#include <stdio.h>

void display_usage(void)
{
    display_purpose();
    printf("\n");
    printf("Usage: ./serverManager <ip> <port> <passcode>\n");
}

void display_purpose(void)
{
    printf("Purpose: This app is meant to simulate a server manager for the     purpose of testing servers.\n");
}

void display_divider(const char *text)
{
    const char *divider = "------";
    printf("\n%-10s %-25s %s\n\n", divider, text, divider);
}
