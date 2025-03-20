//
// Created by ephraim on 3/19/25.
//

#include "../include/server_ip.h"
#include <arpa/inet.h>

pthread_mutex_t  server_ip_mutex                = PTHREAD_MUTEX_INITIALIZER;
char             server_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
_Atomic(uint8_t) server_ip_length;

void initialize_server_ip(void)
{
    atomic_init(&server_ip_length, 7);    // ✅ Correctly initializing atomic variable
}
