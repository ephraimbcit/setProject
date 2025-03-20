//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_FLAG_H
#define SERVER_FLAG_H

#include <pthread.h>

extern pthread_mutex_t server_starter_mutex;
extern int server_starter_connected;

#endif //SERVER_FLAG_H
