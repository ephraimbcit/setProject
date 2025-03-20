//
// Created by ephraim on 3/19/25.
//

#include "../include/server_flag.h"

pthread_mutex_t server_starter_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_starter_connected = 0;
