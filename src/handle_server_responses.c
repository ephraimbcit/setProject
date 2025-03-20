//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_server_responses.h"
#include "../include/setup_connections.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <unistd.h>

#define RESPONSE_HEADER_SIZE 4
#define REQUIRED_PROTOCOL_VERSION 0x03
#define RESPONSE_TYPE_INDEX 0
#define RESPONSE_VERSION_INDEX 1
#define RESPONSE_PAYLOAD_LENGTH_INDEX_1 2
#define RESPONSE_PAYLOAD_LENGTH_INDEX_2 3
#define BIT_SHIFT_BIG_ENDIAN 8
#define SERVER_START_TYPE 0x14
#define ZERO_PLACEHOLDER 0x00
#define START_STOP_HEADER_SIZE 4
#define START_STOP_TYPE_INDEX 0
#define START_STOP_PAYLOAD_VERSION_INDEX 1
#define START_STOP_PAYLOAD_INDEX_1 2
#define START_STOP_PAYLOAD_INDEX_2 3

uint16_t get_payload_length(uint8_t first, uint8_t second);

void parse_response(int type, int server_fd, uint16_t payload_length);

void send_server_start(int server_fd);

void *handle_server_response(void *arg)
{
    struct connection_info *server_info;
    struct starter_info    *starter_data;
    int                     server_fd;

    server_info  = (struct connection_info *)arg;
    starter_data = server_info->starter_data;
    server_fd    = server_info->fd;

    free(arg);

    // artificially make server run~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    send_server_start(server_fd);

    pthread_mutex_lock(&starter_data->starter_mutex);
    starter_data->server_running_flag = 1;
    pthread_mutex_unlock(&starter_data->starter_mutex);

    while(1)
    {
        ssize_t       bytes_recieved;
        unsigned char response_header[RESPONSE_HEADER_SIZE];
        unsigned char response_type;
        uint8_t       response_version;
        uint16_t      response_payload_length;
        uint8_t       length_first_byte;
        uint8_t       length_second_byte;

        bytes_recieved = read(server_fd, response_header, RESPONSE_HEADER_SIZE);

        if(bytes_recieved < RESPONSE_HEADER_SIZE)
        {
            printf("Error reading server response.\n");
            close(server_fd);
            return NULL;
        }

        response_version = response_header[RESPONSE_VERSION_INDEX];

        if(response_version != REQUIRED_PROTOCOL_VERSION)
        {
            printf("Server response version not supported\n");
            close(server_fd);
            return NULL;
        }

        // THERE NEEDS TO BE SOME TYPE OF CHECK THAT WILL EXIT THIS LOOP.

        response_type = response_header[RESPONSE_TYPE_INDEX];

        length_first_byte  = response_header[RESPONSE_PAYLOAD_LENGTH_INDEX_1];
        length_second_byte = response_header[RESPONSE_PAYLOAD_LENGTH_INDEX_2];

        response_payload_length = get_payload_length(length_first_byte, length_second_byte);

        parse_response(response_type, server_fd, response_payload_length);
    }

    return NULL;
}

void send_server_start(int server_fd)
{
    ssize_t       bytes_sent;
    unsigned char server_start_message[START_STOP_HEADER_SIZE];

    server_start_message[START_STOP_TYPE_INDEX]            = SERVER_START_TYPE;
    server_start_message[START_STOP_PAYLOAD_VERSION_INDEX] = REQUIRED_PROTOCOL_VERSION;
    server_start_message[START_STOP_PAYLOAD_INDEX_1]       = ZERO_PLACEHOLDER;
    server_start_message[START_STOP_PAYLOAD_INDEX_2]       = ZERO_PLACEHOLDER;

    bytes_sent = send(server_fd, server_start_message, sizeof(server_start_message), 0);

    if(bytes_sent < START_STOP_HEADER_SIZE)
    {
        printf("Error writing server start.\n");
        close(server_fd);
    }
}

uint16_t get_payload_length(uint8_t first, uint8_t second)
{
    uint16_t length;
    length = (uint16_t)((first << BIT_SHIFT_BIG_ENDIAN) | second);
    return length;
}

void parse_response(int type, int server_fd, uint16_t payload_length)
{
    unsigned char *payload = malloc((size_t)payload_length);
    ssize_t        bytes_read;
    ssize_t        bytes_to_read;

    if(!payload)
    {
        printf("Failed to allocate memory for payload.\n");
        return;
    }

    printf("Type: %d.\n", type);

    bytes_to_read = payload_length;

    bytes_read = read(server_fd, payload, (size_t)bytes_to_read);

    if(bytes_read < payload_length)
    {
        printf("Error reading response payload %d.\n", payload_length);
        free(payload);
        return;
    }

    for(ssize_t i = 0; i < bytes_read; i++)
    {
        printf("%d\n", payload[i]);
    }

    free(payload);
}
