//
// Created by ephraim on 2/11/25.
//

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

// #define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#include "../include/handle_client_requests.h"
#include "../include/setup_connections.h"
#include <handle_server_responses.h>

void *setup_connections(void *arg)
{
    struct connection_info *connection_info;
    int                     fd;
    int                     type;

    struct sockaddr_in address;
    socklen_t          addr_len          = sizeof(address);
    pthread_t          connection_thread = 0;

    connection_info = (struct connection_info *)arg;
    fd              = connection_info->fd;
    type            = connection_info->type;

    free(connection_info);

    while(!exit_flag)
    {
        int  connection_fd;
        int *connection_fd_ptr;

        connection_fd = accept(fd, (struct sockaddr *)&address, &addr_len);
        if(connection_fd < 0)
        {
            if(exit_flag)
            {
                break;
            }
            perror("accept failed");
            continue;
        }

        connection_fd_ptr = (int *)malloc(sizeof(int));

        if(!connection_fd_ptr)
        {
            perror("malloc failed");
            close(connection_fd);
            continue;
        }

        *connection_fd_ptr = connection_fd;

        if(type == TYPE_CLIENT)
        {
            if(pthread_create(&connection_thread, NULL, handle_client, (void *)connection_fd_ptr) != 0)
            {
                perror("Could not create connection thread\n");
                printf("Connection type: %d\n", type);
                close(connection_fd);
                free(connection_fd_ptr);
                continue;
            }
        }

        if(type == TYPE_SERVER)
        {
            printf("about to create server thread");
            if(pthread_create(&connection_thread, NULL, handle_server_response, (void *)connection_fd_ptr) != 0)
            {
                perror("Could not create connection thread\n");
                printf("Connection type: %d\n", type);
                close(connection_fd);
                free(connection_fd_ptr);
                continue;
            }
        }

        pthread_detach(connection_thread);
    }
    pthread_exit(NULL);
}
