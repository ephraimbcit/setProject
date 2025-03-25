//
// Created by brandonr on 12/02/25.
//

#ifndef SETUP_MENU_H
#define SETUP_MENU_H

#include <ncurses.h>
#include <signal.h>
void start_display(void);
int create_colors(void);
extern volatile sig_atomic_t exit_flag;

#endif //SETUP_MENU_H
