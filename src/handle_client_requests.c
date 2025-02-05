//
// Created by ephraim on 2/3/25.
//

#include "../include/handle_client_requests.h"
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVER_MANAGER_RESPONSE_MAX_SIZE 16
#define IP_STARTING_INDEX 5
#define CLIENT_REQUEST_MAX_SIZE 2
#define UTF8STRING_PROTOCOL 0x0C

int randomZeroOrOne(void);

void *handle_client(void *arg)
{
    int                 client_fd;
    int                 server_is_live;
    ssize_t             bytes_recieved;
    unsigned char       client_request[CLIENT_REQUEST_MAX_SIZE];
    const unsigned char server_ip[SERVER_MANAGER_RESPONSE_MAX_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char       server_manager_response[SERVER_MANAGER_RESPONSE_MAX_SIZE];
    unsigned char       client_type;
    unsigned char       client_version;
    unsigned char       server_response_type;
    unsigned char       server_response_version;

    client_fd = *(int *)arg;
    free(arg);
    // Some other helper function that checks if server is running and available.
    server_is_live = randomZeroOrOne();

    server_response_type       = 0x01;
    server_response_version    = 0x01;
    server_manager_response[0] = server_response_type;
    server_manager_response[1] = server_response_version;
    server_manager_response[3] = UTF8STRING_PROTOCOL;

    bytes_recieved = read(client_fd, client_request, 2);

    if(bytes_recieved < 2)
    {
        printf("Invalid request received.\n");
        close(client_fd);
        return NULL;
    }

    client_type    = client_request[0];
    client_version = client_request[1];

    if(client_type != 0x00 || client_version != 0x01)
    {
        printf("Invalid request received.\n");
        close(client_fd);
        return NULL;
    }

    if(server_is_live)
    {
        int     counter;
        ssize_t bytes_sent;

        server_manager_response[2] = 0x01;
        server_manager_response[4] = UTF8STRING_PROTOCOL;

        for(counter = IP_STARTING_INDEX; counter < SERVER_MANAGER_RESPONSE_MAX_SIZE; counter++)
        {
            server_manager_response[counter] = server_ip[counter - IP_STARTING_INDEX];
        }

        bytes_sent = send(client_fd, server_manager_response, sizeof(server_manager_response), 0);

        if(bytes_sent < 0)
        {
            perror("Failed to respond to client");
            close(client_fd);
            return NULL;
        }
    }
    else
    {
        ssize_t bytes_sent;

        server_manager_response[2] = 0x00;
        server_manager_response[4] = 0x00;

        bytes_sent = send(client_fd, server_manager_response, sizeof(server_manager_response), 0);

        if(bytes_sent < 0)
        {
            perror("Failed to respond to client");
            close(client_fd);
            return NULL;
        }
    }
    close(client_fd);
    return NULL;
}

int randomZeroOrOne(void)
{
    int result;

    result = rand() % 2;    // Generates either 0 or 1
    return result;
}
