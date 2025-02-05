//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_requests.h"

void *handle_starter(void *arg)
{
    int starter_fd;

    starter_fd = *(int *)arg;
    printf("Starter fd: %d\n", starter_fd);
    return NULL;
}
