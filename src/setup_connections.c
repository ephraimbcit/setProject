//
// Created by ephraim on 2/11/25.
//

#include "../include/setup_connections.h"
#include "../include/handle_client_requests.h"
#include "../include/handle_server_responses.h"
#include "../include/server_ip.h"
#include "../include/server_status_flags.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

// #define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

void *setup_connections(void *arg)
{
    struct connection_info *connection_info;
    int                     fd;
    int                     type;
    struct sockaddr_in      address;

    socklen_t addr_len          = sizeof(address);
    pthread_t connection_thread = 0;

    connection_info = (struct connection_info *)arg;
    fd              = connection_info->fd;
    type            = connection_info->type;

    free(connection_info);

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
            perror("Failed to accept a connection");
            close(connection_fd);
            continue;
        }

        if(type == TYPE_SERVER && atomic_load(&starter_connected_flag))
        {
            close(connection_fd);
            continue;  // ← Don't break, just ignore this one and keep listening
        }

        new_connection_info = malloc(sizeof(struct connection_info));

        if(!new_connection_info)
        {
            perror("Failed to allocate memory for new_connection_info");
            close(connection_fd);
            continue;
        }

        new_connection_info->fd   = connection_fd;
        new_connection_info->type = type;

        if(type == TYPE_CLIENT)
        {
            if(pthread_create(&connection_thread, NULL, handle_client, (void *)new_connection_info) != 0)
            {
                perror("Failed to create client connection thread");
                close(connection_fd);
                free(new_connection_info);
                continue;
            }
        }

        if(type == TYPE_SERVER)
        {
            // If no server starter is connected then create a new server connection thread and also set the starter_connected_flag to 1
            if(atomic_exchange(&starter_connected_flag, 1) == 0)
            {
                char ip_string[INET_ADDRSTRLEN];

                if(inet_ntop(AF_INET, &address.sin_addr, ip_string, sizeof(ip_string)) == NULL)
                {
                    perror("Failed to convert address to string");
                    // handle error (e.g., close connection_fd)
                    close(connection_fd);
                    free(new_connection_info);
                    // Set the starter_connected_flag to 0 if thread creation fails
                    atomic_store(&starter_connected_flag, 0);
                    continue;
                }

                // Create thread for handling communication with the server starter
                if(pthread_create(&connection_thread, NULL, handle_server_response, (void *)new_connection_info) != 0)
                {
                    perror("Failed to create server connection thread");
                    close(connection_fd);
                    free(new_connection_info);
                    // Set the starter_connected_flag to 0 if thread creation fails
                    atomic_store(&starter_connected_flag, 0);
                    continue;
                }

                set_ip(ip_string);
            }
            else
            {
                // Need to connection info struct here if a starter attempts to connect while one is already connected
                close(connection_fd);
                free(new_connection_info);
            }
        }
        // Detach the connection thread here since we don't need the setup thread to wait on the spawned client and starter threads
        pthread_detach(connection_thread);
    }
    // Ensures that all the threads that this function spawns will finish execution before main exits.
    pthread_exit(NULL);
}
