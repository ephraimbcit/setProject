//
// Created by ephraim on 2/3/25.
//

#include "../include/handle_client_requests.h"
#include "../include/handle_server_responses.h"
#include "../include/setup_connections.h"
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CLIENT_REQUEST_MAX_SIZE 2
#define SERVER_MANAGER_RESPONSE_MAX_SIZE 23
#define NO_ACTIVE_RESPONSE_SIZE 3

#define SERVER_PORT 8080

#define MANAGER_RESPONSE_TYPE_RETURN_IP 0x01
#define VALID_RESPONSE_VERSION 0x03
#define MANAGER_RESPONSE_TYPE_INDEX 0
#define MANAGER_RESPONSE_VERSION_INDEX 1
#define PAYLOAD_TYPE_INDEX 3

#define CLIENT_REQUEST_TYPE_GET_IP 0x00
#define CLIENT_REQUEST_TYPE_INDEX 0
#define CLIENT_REQUEST_VERSION_INDEX 1

#define SERVER_STATUS_INDEX 2
#define SERVER_ACTIVE 0x01
#define SERVER_INACTIVE 0x00

#define IP_LENGTH_PAYLOAD_INDEX 4

#define PORT_LENGTH 4
#define PORT_LENGTH_BYTE 0x04
#define PORT_ASCII_SIZE 5

#define IP_STARTING_INDEX 5

#define UTF8STRING_PROTOCOL 0x0C

void *handle_client(void *arg)
{
    struct connection_info *client_info;
    struct starter_info    *starter_data;
    int                     client_fd;
    int                     server_is_live;
    ssize_t                 bytes_received;
    unsigned char           client_request[CLIENT_REQUEST_MAX_SIZE];
    unsigned char           server_manager_response[SERVER_MANAGER_RESPONSE_MAX_SIZE];
    unsigned char           server_manager_response_no_active[NO_ACTIVE_RESPONSE_SIZE];
    unsigned char           client_type;
    unsigned char           client_version;
    unsigned char           server_response_type;
    unsigned char           server_response_version;
    char                    ip_buffer[INET_ADDRSTRLEN];

    client_info  = (struct connection_info *)arg;
    starter_data = client_info->starter_data;
    client_fd    = client_info->fd;
    free(arg);

    pthread_mutex_lock(&starter_data->starter_mutex);

    // ✅ Check if the server is running
    server_is_live = starter_data->server_running_flag;
    inet_ntop(AF_INET, &(starter_data->starter_address.sin_addr), ip_buffer, INET_ADDRSTRLEN);

    pthread_mutex_unlock(&starter_data->starter_mutex);

    server_response_type    = MANAGER_RESPONSE_TYPE_RETURN_IP;
    server_response_version = VALID_RESPONSE_VERSION;

    server_manager_response[MANAGER_RESPONSE_TYPE_INDEX]    = server_response_type;
    server_manager_response[MANAGER_RESPONSE_VERSION_INDEX] = server_response_version;

    server_manager_response_no_active[MANAGER_RESPONSE_TYPE_INDEX]    = server_response_type;
    server_manager_response_no_active[MANAGER_RESPONSE_VERSION_INDEX] = server_response_version;

    server_manager_response[PAYLOAD_TYPE_INDEX] = UTF8STRING_PROTOCOL;

    bytes_received = read(client_fd, client_request, 2);

    if(bytes_received < 2)
    {
        printf("Invalid request received.\n");
        close(client_fd);
        return NULL;
    }

    client_type    = client_request[CLIENT_REQUEST_TYPE_INDEX];
    client_version = client_request[CLIENT_REQUEST_VERSION_INDEX];

    if(client_type != CLIENT_REQUEST_TYPE_GET_IP || client_version != VALID_RESPONSE_VERSION)
    {
        printf("Invalid request received.\n");
        close(client_fd);
        return NULL;
    }

    if(server_is_live)
    {
        int           ip_length;
        unsigned char ip_length_byte;
        int           counter;
        ssize_t       bytes_sent;
        int           port_type_payload_index;
        int           port_length_payload_index;
        int           port_index;
        char          port_ascii[PORT_ASCII_SIZE];

        ip_length                 = (int)strlen(ip_buffer);
        ip_length_byte            = (unsigned char)ip_length;
        port_type_payload_index   = IP_LENGTH_PAYLOAD_INDEX + ip_length;
        port_length_payload_index = port_type_payload_index + 1;
        port_index                = port_length_payload_index + 1;

        // Set response type and protocol
        server_manager_response[SERVER_STATUS_INDEX]     = SERVER_ACTIVE;
        server_manager_response[IP_LENGTH_PAYLOAD_INDEX] = ip_length_byte;

        // ✅ Copy stored IP into the response (no direct access to `server_ip_str`)
        for(counter = 0; counter < ip_length; counter++)
        {
            server_manager_response[IP_STARTING_INDEX + counter] = (uint8_t)ip_buffer[counter];
        }

        // ✅ Convert port to ASCII hex representation
        snprintf(port_ascii, sizeof(port_ascii), "%04d", SERVER_PORT);

        server_manager_response[port_type_payload_index]   = UTF8STRING_PROTOCOL;
        server_manager_response[port_length_payload_index] = PORT_LENGTH_BYTE;

        // ✅ Copy port ASCII representation
        memcpy(&server_manager_response[port_index], port_ascii, PORT_LENGTH);

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
        ssize_t bytes_sent;

        server_manager_response_no_active[2] = SERVER_INACTIVE;

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
