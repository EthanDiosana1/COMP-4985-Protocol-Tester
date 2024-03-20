#ifndef COMP_4985_PROTOCOL_TESTER_PROTOCOLMOCKSERVER_H
#define COMP_4985_PROTOCOL_TESTER_PROTOCOLMOCKSERVER_H

#include <netinet/in.h>

int create_server(void);
int accept_connection(int server_fd, struct sockaddr_storage *client_addr, socklen_t *client_addr_len);

#endif    // COMP_4985_PROTOCOL_TESTER_PROTOCOLMOCKSERVER_H
