//
// Created by brandonr on 12/02/25.
//

#ifndef HANDLE_MENU_H
#define HANDLE_MENU_H
#include <ncurses.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

int end_display(void);
void update_display(int server_fd);
void *handle_input(void *arg);
void update_info(const char *ip_address, int user_count);



void *test_update_info(void *arg);
void generate_random_ip(char *buffer, size_t size);
void update_server_user_count(const int user_count);
void update_server_message_count(const int message_count);

struct menu
{
  int window_height;
  int window_width;
  int height;
  int width;
  char vertical_border_char;
  char horizontal_border_char;
  int current_selection;
  int server_is_on;
  int user_count;
  int message_count;
  char ip_address[INET_ADDRSTRLEN];
  time_t last_updated_time;


};
// void handle_display(const char *ip_address, int user_count, struct menu *interface);
void handle_display(void);

static pthread_mutex_t menu_mutex;
#endif //HANDLE_MENU_H
