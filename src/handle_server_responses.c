//
// Created by ephraim on 2/4/25.
//

#include "../include/handle_server_responses.h"
#include "../include/handle_menu.h"
#include "../include/server_status_flags.h"
#include "../include/setup_connections.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <unistd.h>

#define RESPONSE_HEADER_SIZE 4
#define REQUIRED_PROTOCOL_VERSION 0x03
#define BIT_SHIFT_ONE_BYTES 8
#define BIT_SHIFT_TWO_BYTES 16
#define BIT_SHIFT_THREE_BYTES 24
#define SERVER_USR_COUNT 0x0A
#define SERVER_START 1
#define SERVER_STOP 0
#define SERVER_START_TYPE 0x14
#define SERVER_STOP_TYPE 0x15
#define SERVER_ONLINE 0x0C
#define SERVER_OFFLINE 0x0D
#define ZERO_PLACEHOLDER 0x00
#define START_STOP_HEADER_SIZE 4
#define ENUM_TYPE_INTEGER 0x02
#define BIG_BUFFER 256

// void send_starter_message(int server_fd, int type);

void set_server_running_flag(int value);

void handle_server_status(uint8_t status);

int get_payload_length(uint8_t high_byte, uint8_t low_byte);

int get_payload_length_32(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

void handle_server_diagnostics(int server_fd, int payload_length, uint8_t type);

void *handle_server_response(void *arg)
{
    struct connection_info *server_info;

    int server_fd;
    int server_communication_flag;

    // Set flag for listening loop.
    server_communication_flag = 1;

    server_info = (struct connection_info *)arg;
    server_fd   = server_info->fd;

    free(server_info);

    // Send start message to server starter (temporary implementation)
    // Will need to link this to one of Brandon's "start server" buttons to manually tell the starter to start a server.
    send_starter_message(server_fd, SERVER_START);

    // Update the menus server_fd
    update_server_connection_info(server_fd);
    //

    // artificially simulate that the server says it's online
    atomic_store(&server_running_flag, 1);

    while(server_communication_flag)
    {
        ssize_t  bytes_recieved;
        uint8_t  response_header[RESPONSE_HEADER_SIZE];
        uint8_t *response_header_ptr;
        uint8_t  response_type;
        uint8_t  response_version;
        int      payload_length;
        uint8_t  high_byte;
        uint8_t  low_byte;

        response_header_ptr = response_header;

        bytes_recieved = read(server_fd, response_header, RESPONSE_HEADER_SIZE);

        if(bytes_recieved < RESPONSE_HEADER_SIZE)
        {
            printf("Bytes received: %d\n", (int)bytes_recieved);
            perror("Error reading server response");
            close(server_fd);
            server_communication_flag = 0;
        }

        response_type = *response_header_ptr++;

        response_version = *response_header_ptr++;

        if(response_version != REQUIRED_PROTOCOL_VERSION)
        {
            perror("Server response version not supported");
            close(server_fd);
            server_communication_flag = 0;
        }

        high_byte = *response_header_ptr++;
        low_byte  = *response_header_ptr;

        payload_length = get_payload_length(high_byte, low_byte);

        if(response_type == SERVER_ONLINE)
        {
            handle_server_status(SERVER_ONLINE);
        }

        if(response_type == SERVER_OFFLINE)
        {
            handle_server_status(SERVER_OFFLINE);
        }

        if(response_type == SERVER_USR_COUNT)
        {
            handle_server_diagnostics(server_fd, payload_length, SERVER_USR_COUNT);
        }
    }
    return NULL;
}

void send_starter_message(int server_fd, int type)
{
    uint8_t *server_start_ptr;
    ssize_t  bytes_sent;
    uint8_t  server_start_message[START_STOP_HEADER_SIZE];

    server_start_ptr = server_start_message;

    if(type == SERVER_START)
    {
        *server_start_ptr++ = SERVER_START_TYPE;
        *server_start_ptr++ = REQUIRED_PROTOCOL_VERSION;
        *server_start_ptr++ = ZERO_PLACEHOLDER;
        *server_start_ptr   = ZERO_PLACEHOLDER;
    }

    if(type == SERVER_STOP)
    {
        *server_start_ptr++ = SERVER_STOP_TYPE;
        *server_start_ptr++ = REQUIRED_PROTOCOL_VERSION;
        *server_start_ptr++ = ZERO_PLACEHOLDER;
        *server_start_ptr   = ZERO_PLACEHOLDER;
    }

    bytes_sent = send(server_fd, server_start_message, sizeof(server_start_message), 0);

    if(bytes_sent < START_STOP_HEADER_SIZE)
    {
        perror("Failed to send server start message");
        close(server_fd);
    }
}

void set_server_running_flag(int value)
{
    atomic_store(&server_running_flag, value);
    if(!(atomic_load(&server_running_flag) == value))
    {
        perror("Failed to set server flag");
    }
}

void handle_server_status(uint8_t status)
{
    if(status == SERVER_ONLINE)
    {
        set_server_running_flag(SERVER_START);
    }

    if(status == SERVER_OFFLINE)
    {
        set_server_running_flag(SERVER_STOP);
    }
}

int get_payload_length(uint8_t high_byte, uint8_t low_byte)
{
    uint16_t value;

    value = (uint16_t)(high_byte << BIT_SHIFT_ONE_BYTES) | low_byte;

    return (int)value;
}

int get_payload_length_32(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    uint32_t value;
    // Combine the four bytes into a 32-bit integer assuming big-endian order:
    value = ((uint32_t)b1 << BIT_SHIFT_THREE_BYTES) | ((uint32_t)b2 << BIT_SHIFT_TWO_BYTES) | ((uint32_t)b3 << BIT_SHIFT_ONE_BYTES) | b4;

    return (int)value;
}

void handle_server_diagnostics(int server_fd, int payload_length, uint8_t type)
{
    ssize_t  bytes_read;
    uint8_t  buffer[BIG_BUFFER];
    uint8_t *buffer_ptr;
    uint8_t  user_count_type;
    uint8_t  user_count_high_byte;
    uint8_t  user_count_low_byte;
    int      user_count;
    uint8_t  message_count_type;
    uint8_t  message_count_b1;
    uint8_t  message_count_b2;
    uint8_t  message_count_b3;
    uint8_t  message_count_b4;
    int      message_count;

    bytes_read = 0;

    if(type == SERVER_USR_COUNT)
    {
        bytes_read = read(server_fd, buffer, (size_t)payload_length);
    }

    if(bytes_read < payload_length)
    {
        perror("Failed to read server diagnostics");
        return;
    }

    buffer_ptr = buffer;

    user_count_type = *buffer_ptr++;

    if(!(user_count_type == ENUM_TYPE_INTEGER))
    {
        perror("User count payload type is not an integer");
        return;
    }

    // Increment to skip over user count payload length, this may need to be changed
    buffer_ptr++;

    // This gets the two bytes that represent the user count
    user_count_high_byte = *buffer_ptr++;
    user_count_low_byte  = *buffer_ptr++;

    // Using a helper function to get the user count since it uses the same logic
    user_count = get_payload_length(user_count_high_byte, user_count_low_byte);
    //
    update_server_user_count(user_count);    // Update the user counts on the menu.
    //

    fprintf(stderr, "User count: %d\n", user_count);

    message_count_type = *buffer_ptr++;

    if(!(message_count_type == ENUM_TYPE_INTEGER))
    {
        perror("Message count payload type is not an integer");
        return;
    }

    // Increment to skip over message count payload length, this may need to be changed
    buffer_ptr++;

    message_count_b1 = *buffer_ptr++;
    message_count_b2 = *buffer_ptr++;
    message_count_b3 = *buffer_ptr++;
    message_count_b4 = *buffer_ptr;

    message_count = get_payload_length_32(message_count_b1, message_count_b2, message_count_b3, message_count_b4);
    //
    update_server_message_count(message_count);    // Update the message counts on the menu.
    //

    fprintf(stderr, "Message count: %d\n", message_count);
}
