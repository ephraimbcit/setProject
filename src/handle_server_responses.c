//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_server_responses.h"
#include <unistd.h>


#define RESPONSE_HEADER_SIZE 4
#define SERVER_MANAGER_REQUEST_MAX_SIZE 2
#define REQUIRED_PROTOCOL_VERSION 0x01
#define UTF8STRING_PROTOCOL 0x0C
#define USER_COUNT 0x0A
#define ACTIVE_USER_COUNT 0x0B
#define RESPONSE_TYPE_INDEX 0
#define RESPONSE_VERSION_INDEX 1
#define RESPONSE_PAYLOAD_LENGTH_INDEX 3

void parse_response(int type, void *server_fd_ptr, int payload_length)
{
    int server_fd;
    unsigned char response[payload_length + RESPONSE_HEADER_SIZE];
    unsigned char payload[payload_length];
    ssize_t bytes_read;
    ssize_t bytes_to_read;

    server_fd = *(int*)server_fd_ptr;

    bytes_to_read = payload_length + RESPONSE_HEADER_SIZE;

    bytes_read = read(server_fd, &response, (size_t)bytes_to_read);

    if(bytes_read < 4)
    {
        printf("Error reading response payload.\n");
        return;
    }

    for ()
}

void *handle_server_response(void *arg)
{
    int                 server_fd;
    ssize_t             bytes_recieved;
    unsigned char       server_response_header[RESPONSE_HEADER_SIZE];
    unsigned char       server_response_type;
    unsigned char       server_response_version;
    unsigned char       server_response_payload_length;

    server_fd = *(int *)arg;
    free(arg);

    bytes_recieved = read(server_fd, server_response_header, RESPONSE_HEADER_SIZE);

    if(bytes_recieved < 4)
    {
        printf("Error reading server response.\n");
        close(server_fd);
        return NULL;
    }

    server_response_version = server_response_header[RESPONSE_VERSION_INDEX];

    if (server_response_version != REQUIRED_PROTOCOL_VERSION)
    {
        printf("Server response version not supported\n");
        close(server_fd);
        return NULL;
    }

    server_response_type = server_response_header[RESPONSE_TYPE_INDEX];

    server_response_payload_length = server_response_header[RESPONSE_PAYLOAD_LENGTH_INDEX];

    parse_response(server_response_type, &server_fd, server_response_payload_length);

    close(server_fd);
    return NULL;
}
