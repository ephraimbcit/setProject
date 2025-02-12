//
// Created by ephraim on 2/11/25.
//

#include "../include/setup_listener.h"
#include "../include/setup_helper.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#define STARTER_PORT 8090
#define SERVER_PORT 9000
#define CLIENT_PORT 8080

int setup_listener(int *fd, int type)
{
    socklen_t          addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in address;

    if(type == TYPE_STARTER)
    {
        setup_socket(fd);
        setup_address(&address, &addr_len, STARTER_PORT);
    }

    if(type == TYPE_SERVER)
    {
        setup_socket(fd);
        setup_address(&address, &addr_len, SERVER_PORT);
    }

    if(type == TYPE_CLIENT)
    {
        setup_socket(fd);
        setup_address(&address, &addr_len, CLIENT_PORT);
    }

    if(bind(*fd, (struct sockaddr *)&address, addr_len) != 0)
    {
        perror("Bind failed");
        printf("Bind type: %d\nError: %d\n", type, errno);
        close(*fd);
        return EXIT_FAILURE;
    }

    if(listen(*fd, SOMAXCONN) != 0)
    {
        perror("Listen failed");
        printf("Listen type: %d\nError: %d\n", type, errno);
        close(*fd);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
