//
// Created by ephraim on 2/11/25.
//

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

// #define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#include "../include/handle_client_requests.h"
#include "../include/handle_server_responses.h"
#include "../include/server_flag.h"
#include "../include/server_ip.h"
#include "../include/server_running_flag.h"
#include "../include/setup_connections.h"

void set_starter_flag(int value);

void update_server_ip(struct sockaddr_in *server_addr);

void *setup_connections(void *arg)
{
    int starter_connected;

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
            close(connection_fd);
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
            // Unlock server starter flag to check it
            pthread_mutex_lock(&server_starter_mutex);
            starter_connected = server_starter_connected;
            pthread_mutex_unlock(&server_starter_mutex);

            if(!starter_connected)    // only connect if there isn't already a server starter connected
            {
                set_starter_flag(1);

                update_server_ip(&address);

                if(pthread_create(&connection_thread, NULL, handle_server_response, (void *)connection_fd_ptr) != 0)
                {
                    perror("Could not create connection thread\n");
                    printf("Connection type: %d\n", type);
                    close(connection_fd);
                    free(connection_fd_ptr);
                    set_starter_flag(0);
                    continue;
                }
            }
            else
            {
                free(connection_fd_ptr);
            }
        }

        pthread_detach(connection_thread);
    }
    pthread_exit(NULL);
}

void set_starter_flag(int value)
{
    pthread_mutex_lock(&server_starter_mutex);
    server_starter_connected = value;
    pthread_mutex_unlock(&server_starter_mutex);
}

void update_server_ip(struct sockaddr_in *server_addr)
{
    pthread_mutex_lock(&server_ip_mutex);

    inet_ntop(AF_INET, &(server_addr->sin_addr), server_ip_str, INET_ADDRSTRLEN);

    atomic_store(&server_ip_length, strlen(server_ip_str));

    pthread_mutex_unlock(&server_ip_mutex);
}
