#include "../include/protocolClientTests.h"
#include "../include/protocolMockServer.h"
#include <unistd.h>

int testProtocolClient(void)
{
    int server_fd;

    server_fd = create_server();


    close(server_fd);
    return 0;
}
