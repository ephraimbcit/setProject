//
// Created by ephraim on 2/3/25.
//

#include "../include/handle_client_requests.h"
#include "../include/handle_server_responses.h"
#include "../include/setup_connections.h"
#include <netinet/in.h>
#include <pthread.h>
#include <server_ip.h>
#include <server_status_flags.h>
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT "8000"

#define CLIENT_REQUEST_MAX_SIZE 2
#define SERVER_MANAGER_RESPONSE_MAX_SIZE 32
#define NO_ACTIVE_RESPONSE_SIZE 3
#define MANAGER_RESPONSE_TYPE_RETURN_IP 0x01
#define VALID_RESPONSE_VERSION 0x03
#define CLIENT_REQUEST_TYPE_GET_IP 0x00
#define SERVER_ACTIVE 0x01
#define SERVER_INACTIVE 0x00
#define PORT_LENGTH 4
#define PORT_LENGTH_BYTE 0x04
#define UTF8STRING_PROTOCOL 0x0C

void *handle_client(void *arg)
{
    struct connection_info *client_info;

    int            client_fd;
    ssize_t        bytes_received;
    uint8_t        client_request[CLIENT_REQUEST_MAX_SIZE];
    uint8_t        server_manager_response[SERVER_MANAGER_RESPONSE_MAX_SIZE];
    uint8_t        server_manager_response_no_active[NO_ACTIVE_RESPONSE_SIZE];
    uint8_t        client_type;
    uint8_t        client_version;
    const uint8_t *client_request_ptr;

    memset(server_manager_response, 0, SERVER_MANAGER_RESPONSE_MAX_SIZE);

    client_info = (struct connection_info *)arg;
    client_fd   = client_info->fd;
    free(client_info);

    bytes_received = read(client_fd, client_request, 2);

    if(bytes_received < 2)
    {
        printf("Invalid request received.\n");
        close(client_fd);
        return NULL;
    }

    client_request_ptr = client_request;

    client_type    = *client_request_ptr++;
    client_version = *client_request_ptr;

    if(client_type != CLIENT_REQUEST_TYPE_GET_IP || client_version != VALID_RESPONSE_VERSION)
    {
        printf("Invalid client request received.\n");
        close(client_fd);
        return NULL;
    }

    if(atomic_load(&server_running_flag))
    {
        uint8_t *manager_response_ptr;
        char     active_ip[INET_ADDRSTRLEN];
        int      ip_length;
        ssize_t  bytes_sent;

        manager_response_ptr = server_manager_response;

        *manager_response_ptr++ = MANAGER_RESPONSE_TYPE_RETURN_IP;
        *manager_response_ptr++ = VALID_RESPONSE_VERSION;
        *manager_response_ptr++ = SERVER_ACTIVE;

        get_ip(active_ip, sizeof(active_ip));

        ip_length = (int)strlen(active_ip);

        printf("ip len: %d\n", ip_length);

        *manager_response_ptr++ = UTF8STRING_PROTOCOL;
        *manager_response_ptr++ = (uint8_t)ip_length;

        memcpy(manager_response_ptr, active_ip, (unsigned long)ip_length);

        manager_response_ptr += ip_length;

        *manager_response_ptr++ = UTF8STRING_PROTOCOL;
        *manager_response_ptr++ = PORT_LENGTH_BYTE;

        // ✅ Copy port ASCII representation
        memcpy(manager_response_ptr, &SERVER_PORT, PORT_LENGTH);

        // ✅ Send the response
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
        uint8_t *manager_response_ptr;
        ssize_t  bytes_sent;

        manager_response_ptr = server_manager_response_no_active;

        *manager_response_ptr++ = MANAGER_RESPONSE_TYPE_RETURN_IP;
        *manager_response_ptr++ = VALID_RESPONSE_VERSION;
        *manager_response_ptr   = SERVER_INACTIVE;

        bytes_sent = send(client_fd, server_manager_response_no_active, sizeof(server_manager_response_no_active), 0);

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
