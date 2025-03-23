//
// Created by ephraim on 3/22/25.
//

#include "server_ip.h"

// ip_manager.c
#include <string.h>

pthread_rwlock_t ip_rwlock = PTHREAD_RWLOCK_INITIALIZER;
char shared_ip[INET_ADDRSTRLEN] = "";

void set_ip(const char *new_ip) {
    pthread_rwlock_wrlock(&ip_rwlock);
    strncpy(shared_ip, new_ip, INET_ADDRSTRLEN - 1);
    shared_ip[INET_ADDRSTRLEN - 1] = '\0';
    pthread_rwlock_unlock(&ip_rwlock);
}

void get_ip(char *buffer, size_t size) {
    pthread_rwlock_rdlock(&ip_rwlock);
    strncpy(buffer, shared_ip, size - 1);
    buffer[size - 1] = '\0';
    pthread_rwlock_unlock(&ip_rwlock);
}
