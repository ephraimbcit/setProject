//
// Created by ephraim on 3/19/25.
//

#include "../include/server_ip.h"
#include <arpa/inet.h>
#include <string.h>

// Keep variables `static` to prevent global access
static pthread_mutex_t  server_ip_mutex                = PTHREAD_MUTEX_INITIALIZER;
static char             server_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
static _Atomic(uint8_t) server_ip_length;

void initialize_server_ip(void)
{
    atomic_init(&server_ip_length, 7);
}

// Getter function for safely retrieving the stored IP string
void get_server_ip(char *dest, size_t dest_size)
{
    pthread_mutex_lock(&server_ip_mutex);
    strncpy(dest, server_ip_str, dest_size);
    pthread_mutex_unlock(&server_ip_mutex);
}

// Setter function to update the IP string
void set_server_ip(const char *new_ip)
{
    pthread_mutex_lock(&server_ip_mutex);
    strncpy(server_ip_str, new_ip, INET_ADDRSTRLEN);
    atomic_store(&server_ip_length, strlen(new_ip));
    pthread_mutex_unlock(&server_ip_mutex);
}

// Getter for `server_ip_length`
uint8_t get_server_ip_length(void)
{
    return atomic_load(&server_ip_length);
}
