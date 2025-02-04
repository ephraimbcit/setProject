//
// Created by ephraim on 2/3/25.
//

#include "../include/handle_client_requests.h"

void *handle_client(void *arg)
{
    int client_fd;

    client_fd = *(int *)arg;

    printf("Client requested server IP address\n");
    printf("Client fd: %d\n", client_fd);
    return NULL;
}
