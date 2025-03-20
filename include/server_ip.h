//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_IP_H
#define SERVER_IP_H

#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

void initialize_server_ip(void);
void get_server_ip(char *dest, size_t dest_size);
void set_server_ip(const char *new_ip);
uint8_t get_server_ip_length(void);

#endif //SERVER_IP_H
