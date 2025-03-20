//
// Created by ephraim on 2/11/25.
//

#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

extern volatile sig_atomic_t exit_flag;

struct connection_info
{
  int fd;
  int type;
};

void *setup_connections(void *arg);

int is_server_running(void);

void set_server_running(int value);