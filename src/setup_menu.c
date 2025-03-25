//
// Created by brandonr on 12/02/25.
//
#include "setup_menu.h"
#include "handle_menu.h"
#include <errno.h>
#include <pthread.h>
#include <setup_listener.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define TWELVE 12
#define FOURTEEN 14
// #define TOTAL_HEIGHT 13
#define CENTERING_VAL 25

static void setup_signal_handler(void);

static void sigint_handler(int sig_num);

volatile sig_atomic_t exit_flag = 0;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(void)
{
    pthread_t input_thread;    // Thread for handling menu

    setup_signal_handler();

    //---------------- start ncurses display.
    start_display();
    //----------------

    // Input handler thread
    if(pthread_create(&input_thread, NULL, handle_input, NULL) != 0)
    {
        perror("Failed to create input thread");
        return EXIT_FAILURE;
    }

    pthread_detach(input_thread);

    while(!exit_flag)
    {
        sleep(1);
    }

    return 0;
}

void start_display(void)
{
    int height;
    int width;
    int box_top;
    int box_bottom;
    // int box_left;
    // int box_right;

    initscr();               // Initialize ncurses
    noecho();                // Disable automatic echoing of input
    curs_set(0);             // Hide cursor
    keypad(stdscr, TRUE);    // Enable function keys
    nodelay(stdscr, TRUE);
    create_colors();

    clear();      // Clear the screen
    refresh();    // Refresh screen to apply changes

    getmaxyx(stdscr, height, width);    // Get terminal size

    height     = FOURTEEN;
    box_top    = 0;
    box_bottom = box_top + height;

    // Draw top and bottom borders
    for(int x = 0; x <= width; x++)
    {
        mvaddch(box_top, x, '-');       // Top border
        mvaddch(box_bottom, x, '-');    // Bottom border
    }

    // Draw left and right borders
    for(int y = box_top + 1; y < box_bottom; y++)
    {
        mvaddch(y, 0, '|');            // Left border
        mvaddch(y, width - 1, '|');    // Right border
    }

    // Title
    mvprintw(box_top - 1, (width - CENTERING_VAL) / 2, "Server Connection Manager");
    refresh();
}

int create_colors(void)
{
    start_color();
    // Check if the terminal supports colors
    if(!has_colors())
    {
        endwin();
        printf("Your terminal does not support color.\n");
        return 1;
    }

    init_pair(1, COLOR_RED, COLOR_BLACK);      // Not running
    init_pair(2, COLOR_GREEN, COLOR_BLACK);    // Running
    init_pair(3, COLOR_BLACK, COLOR_WHITE);    // Highlighted selection
    return 0;
}

static void setup_signal_handler(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
    sa.sa_handler = sigint_handler;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Setup signal handler failed");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void sigint_handler(int sig_num)
{
    const char *shutdown_msg = "Server manager shutting down.\n";
    // End the ncurses display
    end_display();
    exit_flag = 1;
    write(1, shutdown_msg, strlen(shutdown_msg) + 1);
}

#pragma GCC diagnostic pop
