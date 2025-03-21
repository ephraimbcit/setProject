//
// Created by ephraim on 2/11/25.
//

#include <signal.h>

extern volatile sig_atomic_t exit_flag;

struct connection_info
{
  int fd;
  int type;
};

void *setup_connections(void *arg);
