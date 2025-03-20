//
// Created by ephraim on 2/11/25.
//

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

// #define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#include "../include/handle_client_requests.h"
#include "../include/handle_server_responses.h"
#include "../include/setup_connections.h"

void *setup_connections(void *arg)
{
    struct connection_info *connection_info;
    int                     fd;
    int                     type;
    struct starter_info    *starter_data;

    struct sockaddr_in address;
    socklen_t          addr_len          = sizeof(address);
    pthread_t          connection_thread = 0;

    connection_info = (struct connection_info *)arg;
    fd              = connection_info->fd;
    type            = connection_info->type;
    starter_data    = connection_info->starter_data;

    free(connection_info);

    printf("pre setup connections loop\n");

    pthread_mutex_lock(&starter_data->starter_mutex);
    starter_data->starter_flag        = 0;
    starter_data->server_running_flag = 0;
    pthread_mutex_unlock(&starter_data->starter_mutex);

    while(!exit_flag)
    {
        int                     connection_fd;
        struct connection_info *new_connection_info;

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

        new_connection_info = malloc(sizeof(struct connection_info));

        if(!new_connection_info)
        {
            perror("malloc failed");
            close(connection_fd);
            continue;
        }

        new_connection_info->fd           = connection_fd;
        new_connection_info->type         = type;
        new_connection_info->starter_data = starter_data;

        printf("new connection info struct has assigned valued\n");

        printf("connection type: %d\n", type);

        if(type == TYPE_CLIENT)
        {
            if(pthread_create(&connection_thread, NULL, handle_client, (void *)new_connection_info) != 0)
            {
                perror("Could not create connection thread\n");
                printf("Connection type: %d\n", type);
                close(connection_fd);
                free(new_connection_info);
                continue;
            }
        }

        if(type == TYPE_SERVER)
        {
            int temp_flag;
            pthread_mutex_lock(&starter_data->starter_mutex);
            temp_flag = starter_data->starter_flag;
            pthread_mutex_unlock(&starter_data->starter_mutex);

            printf("starter flag: %d\n", temp_flag);

            if(!temp_flag)    // placeholder for starter_flag check   // only connect if there isn't already a server starter connected
            {
                // Lock mutex and change data in starter_info struct
                pthread_mutex_lock(&starter_data->starter_mutex);
                starter_data->starter_flag             = 1;
                starter_data->server_running_flag      = 1;
                starter_data->starter_address.sin_addr = (&address)->sin_addr;
                pthread_mutex_unlock(&starter_data->starter_mutex);

                if(pthread_create(&connection_thread, NULL, handle_server_response, (void *)new_connection_info) != 0)
                {
                    perror("Could not create connection thread\n");
                    printf("Connection type: %d\n", type);
                    close(connection_fd);
                    free(new_connection_info);
                    // Change starter flag if thread creation fails
                    pthread_mutex_lock(&starter_data->starter_mutex);
                    starter_data->starter_flag        = 0;
                    starter_data->server_running_flag = 0;
                    pthread_mutex_unlock(&starter_data->starter_mutex);
                    continue;
                }

                printf("Server thread started properly\n");
            }
            else
            {
                free(new_connection_info);
            }
        }

        pthread_detach(connection_thread);
    }
    pthread_exit(NULL);
}
