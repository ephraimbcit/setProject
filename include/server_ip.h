//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_IP_H
#define SERVER_IP_H

#include <pthread.h>
#include <stdint.h>

extern pthread_mutex_t server_ip_mutex;
extern char server_ip_str[INET_ADDRSTRLEN];
extern uint8_t server_ip_length;

#endif //SERVER_IP_H
