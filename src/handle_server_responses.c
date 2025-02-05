//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_server_responses.h"

void *handle_server_responses(void *arg)
{
    int server_fd;

    server_fd = *(int *)arg;
    printf("%d\n", server_fd);
    return NULL;
}
