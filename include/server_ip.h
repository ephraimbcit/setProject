//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_IP_H
#define SERVER_IP_H

#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

extern pthread_mutex_t server_ip_mutex;
extern char server_ip_str[INET_ADDRSTRLEN];
extern _Atomic(uint8_t) server_ip_length;

void initialize_server_ip(void);

#endif //SERVER_IP_H
