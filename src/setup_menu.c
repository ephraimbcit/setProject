//
// Created by brandonr on 12/02/25.
//
#include "setup_menu.h"

// #define TWELVE 12
#define FOURTEEN 14
// #define TOTAL_HEIGHT 13
#define CENTERING_VAL 25

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
