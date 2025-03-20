//
// Created by ephraim on 3/19/25.
//

#ifndef SERVER_RUNNING_FLAG_H
#define SERVER_RUNNING_FLAG_H

#include <pthread.h>

extern pthread_mutex_t server_running_mutex;
extern int server_running;

#endif //SERVER_RUNNING_FLAG_H
