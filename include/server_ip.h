//
// Created by ephraim on 3/22/25.
//

#ifndef SERVER_IP_H
#define SERVER_IP_H

#include <pthread.h>
#define INET_ADDRSTRLEN 16  // For example, for IPv4

extern pthread_rwlock_t ip_rwlock;
extern char shared_ip[INET_ADDRSTRLEN];

void set_ip(const char *new_ip);
void get_ip(char *buffer, size_t size);

#endif //SERVER_IP_H
