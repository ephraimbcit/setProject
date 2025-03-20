//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_FLAG_H
#define SERVER_FLAG_H

#include <pthread.h>

pthread_mutex_t server_starter_mutex;
int server_starter_connected = 0;

#endif //SERVER_FLAG_H
