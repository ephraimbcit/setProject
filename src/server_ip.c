//
// Created by ephraim on 3/19/25.
//

#include "../include/server_ip.h"
#include <arpa/inet.h>

pthread_mutex_t server_ip_mutex                = PTHREAD_MUTEX_INITIALIZER;
char            server_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
uint8_t         server_ip_length               = 7;
