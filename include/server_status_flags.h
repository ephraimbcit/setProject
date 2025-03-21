//
// Created by ephraim on 3/20/25.
//

// #include <stdatomic.h>

#ifndef SERVER_STATUS_FLAGS_H
#define SERVER_STATUS_FLAGS_H

extern _Atomic(int) starter_connected_flag;
extern _Atomic(int) server_running_flag;

#endif //SERVER_STATUS_FLAGS_H
