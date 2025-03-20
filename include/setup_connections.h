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
  struct starter_info *starter_data;
};

struct starter_info
{
  int starter_flag;  // Atomic flag for tracking starter connection
  int server_running_flag;
  struct sockaddr_in starter_address;  // Store the starter's IP
  pthread_mutex_t starter_mutex;  // Mutex for safely accessing starter_address
};

void *setup_connections(void *arg);
