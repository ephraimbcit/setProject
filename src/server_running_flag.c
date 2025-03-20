//
// Created by ephraim on 3/19/25.
//

#include "../include/server_running_flag.h"

pthread_mutex_t server_running_mutex = PTHREAD_MUTEX_INITIALIZER;
int             server_running       = 0;
