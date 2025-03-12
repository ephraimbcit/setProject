//
// Created by brandonr on 12/02/25.
//

#ifndef HANDLE_MENU_H
#define HANDLE_MENU_H
#include <ncurses.h>
int end_display(void);
void update_display(int server_fd);
void *handle_input(void *arg);
struct menu
{
  int window_height;
  int window_width;
  int height;
  int width;
  char vertical_border_char;
  char horizontal_border_char;


};

#endif //HANDLE_MENU_H
