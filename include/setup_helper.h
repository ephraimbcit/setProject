//
// Created by ephraim on 2/3/25.
//

#include <netinet/in.h>

void setup_socket(int *fd);
void setup_address(struct sockaddr_in *address, socklen_t *address_len, in_port_t port);
