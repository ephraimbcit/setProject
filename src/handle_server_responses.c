//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_server_responses.h"
#include <stdint.h>
#include <unistd.h>

#define RESPONSE_HEADER_SIZE 4
#define REQUIRED_PROTOCOL_VERSION 0x01
// #define UTF8STRING_PROTOCOL 0x0C
// #define USER_COUNT 0x0A
// #define ACTIVE_USER_COUNT 0x0B
#define RESPONSE_TYPE_INDEX 0
#define RESPONSE_VERSION_INDEX 1
#define RESPONSE_PAYLOAD_LENGTH_INDEX_1 2
#define RESPONSE_PAYLOAD_LENGTH_INDEX_2 3
// #define RESPONSE_PAYLOAD_TYPE_INDEX 0
// #define RESPONSE_PAYLOAD_DATA_LENGTH_INDEX 1
#define BIT_SHIFT_BIG_ENDIAN 8

uint16_t get_payload_length(uint8_t first, uint8_t second);

void parse_response(int type, int server_fd, uint16_t payload_length);

// void handle_diagnostics();

// void handle_();

// void handle_manager_response();

// void get_data_type();
//
// void get_data_length();

void *handle_server_response(void *arg)
{
    int           server_fd;
    ssize_t       bytes_recieved;
    unsigned char response_header[RESPONSE_HEADER_SIZE];
    unsigned char response_type;
    uint8_t       response_version;
    uint16_t      response_payload_length;
    uint8_t       length_first_byte;
    uint8_t       length_second_byte;

    server_fd = *(int *)arg;
    free(arg);

    printf("handling server response:");

    bytes_recieved = read(server_fd, response_header, RESPONSE_HEADER_SIZE);

    if(bytes_recieved < 4)
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

    response_type = response_header[RESPONSE_TYPE_INDEX];

    length_first_byte  = response_header[RESPONSE_PAYLOAD_LENGTH_INDEX_1];
    length_second_byte = response_header[RESPONSE_PAYLOAD_LENGTH_INDEX_2];

    response_payload_length = get_payload_length(length_first_byte, length_second_byte);

    parse_response(response_type, server_fd, response_payload_length);

    return NULL;
}

uint16_t get_payload_length(uint8_t first, uint8_t second)
{
    uint16_t length;
    length = (uint16_t)((first << BIT_SHIFT_BIG_ENDIAN) | second);
    return length;
}

void parse_response(int type, int server_fd, uint16_t payload_length)
{
    unsigned char *payload = malloc(payload_length);
    ssize_t        bytes_read;
    ssize_t        bytes_to_read;
    if(!payload)
    {
        printf("Failed to allocate memory for payload.\n");
        return;
    }

    bytes_to_read = payload_length;

    bytes_read = read(server_fd, payload, (size_t)bytes_to_read);

    printf("Received %d bytes\n", (int)bytes_read);

    if(bytes_read < payload_length)
    {
        printf("Error reading response payload %d.\n", payload_length);
        free(payload);
        return;
    }

    // result = (int)payload[TEST_INDEX];

    printf("Received response payload type %d:\n", type);
    //
    // printf("User count: %d\n", result);

    printf("Received %d bytes\n", (int)bytes_read);
    printf("Raw payload data:\n");

    // Print each byte in hexadecimal format
    for(ssize_t i = 0; i < bytes_read; i++)
    {
        printf("%02X ", payload[i]);    // Print each byte as a 2-digit hex value
    }

    printf("\n");    // Newline after all bytes are printed

    free(payload);
}
