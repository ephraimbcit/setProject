//
// Created by ephraim on 3/22/25.
//

#ifndef SERVER_IP_H
#define SERVER_IP_H

#include <pthread.h>
#include <arpa/inet.h>

extern pthread_rwlock_t ip_rwlock;
extern char shared_ip[INET_ADDRSTRLEN];

void set_ip(const char *new_ip);
void get_ip(char *buffer, size_t size);

#endif //SERVER_IP_H
