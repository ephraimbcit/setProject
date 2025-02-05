//
// Created by ephraim on 2/3/25.
//

#include "../include/setup_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup_socket(int *fd)
{
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if(*fd == -1)
    {
        perror("Error during socket creation:\n");
        exit(EXIT_FAILURE);
    }
}

void setup_address(struct sockaddr_in *address, socklen_t *address_len, in_port_t port)
{
    memset(address, 0, sizeof(*address));

    address->sin_family      = AF_INET;
    address->sin_port        = htons(port);
    address->sin_addr.s_addr = htonl(INADDR_ANY);

    *address_len = sizeof(*address);
}
