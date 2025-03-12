//
// Created by brandonr on 12/02/25.
//

#include "handle_menu.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <setup_connections.h>
#include <stdio.h>
#include <sys/socket.h>
#include <time.h>

// #define FIVE 5
// #define SIX 6
// #define SEVEN 7
// #define TEN 10
// #define TWELVE 12
// #define THIRTEEN 13
#define KEY_PRESSED_LINE 16
#define FOURTEEN 14
#define FIFTEEN 15
#define IP_LINE 1              // Display the ip of the connected server
#define SERVER_ALIVE_LINE 2    // Display a message for the alive status of the server
#define USER_COUNT_LINE 3      // Display the user count and the last time it was updated
#define STARTUP_BUTTON 12      // Display the server startup button
#define SHUTDOWN_BUTTON 13     // Display the server shutdown button
#define TIME_BUFFER_SIZE 20    // Time buffer size for formatted local time array
#define MAX_USERS 31           // Max number of users

#define SLEEP_LENGTH 50000000L

int end_display(void)
{
    endwin();
    return 1;
}

// display IP address ,user count (last updated, recieved update), add a shutdown button and startup, message for when the server is alve or dead
// void update_display(string ip_address,int user_count)
// {
// Based on the newest time the user_count was sent, update a time variable to display the last time it was updated
//     struct sockaddr_in address;
//     socklen_t          addr_len = sizeof(address);
//     static int         row      = 1;
//
//     if(getpeername(server_fd, &address, &addr_len) == -1)
//     {
//         perror("getpeername failed");
//         return;
//     }
//     mvprintw(row, 0, "Connected: %s:%d", inet_ntoa(address.sin_addr), htons(address.sin_port));
//     row++;
//
//     refresh();
// }

void *handle_input(void *arg)
{
    struct timespec sleep_time;
    struct menu     interface;
    char            time_buffer[TIME_BUFFER_SIZE];    // Buffer to store formatted time
    int             current_selection = 1;
    int             height_too_short  = 0;
    time_t          last_update_time  = 0;
    int             server_is_on      = 0;    // 1 is on, 0 is off
    struct tm       time_info;
    int             user_count = 0;

    (void)arg;
    sleep_time.tv_sec  = 0;
    sleep_time.tv_nsec = SLEEP_LENGTH;

    getmaxyx(stdscr, interface.window_height, interface.window_width);    // Get terminal size
    interface.height = FOURTEEN;
    interface.width  = interface.window_width;

    while(!exit_flag)    // Keep handling input until exit
    {
        // int new_window_height;
        // int new_window_width;
        int ch = getch();

        if(getmaxy(stdscr) < interface.height)
        {
            height_too_short = 1;
        }
        else if(height_too_short == 1)
        {
            height_too_short = 0;
        }

        if(ch != ERR)
        {
            switch(ch)
            {
                case 'q':
                    end_display();
                    raise(SIGINT);
                    break;
                case KEY_UP:
                    // mvprintw(FIVE, 1, "Up key pressed!  ");
                    if((current_selection - 1) != 0)
                    {
                        current_selection--;
                    }
                    refresh();
                    break;
                case KEY_DOWN:
                    // mvprintw(SIX, 1, "Down key pressed!");
                    if((current_selection + 1) != FOURTEEN)
                    {
                        current_selection++;
                    }
                    refresh();
                    break;
                case '\n':
                    // mvprintw(SIX, 1, "Down key pressed!");
                    if(current_selection == STARTUP_BUTTON)
                    {
                        server_is_on = 1;
                    }
                    else if(current_selection == SHUTDOWN_BUTTON)
                    {
                        server_is_on = 0;
                    }
                    refresh();
                    break;
                case 'k':    // Get current time when 'k' is pressed
                    if(server_is_on == 1)
                    {
                        user_count       = rand() % MAX_USERS;
                        last_update_time = time(NULL);
                    }
                    break;
                case 'p':    // Get current time when 'k' is pressed
                    server_is_on = !server_is_on;
                    break;
                default:
                    mvprintw(KEY_PRESSED_LINE, 1, "Pressed: %c ", ch);
                    refresh();
            }
        }
        clear();
        mvprintw(FIFTEEN, 1, "Current selection: %d ", current_selection);
        for(int x = 0; x < interface.width; x++)
        {
            mvaddch(0, x, '-');
            mvaddch(interface.height, x, '-');
        }
        for(int y = 0; y <= interface.height; y++)
        {
            if(y != interface.height && y != 0)
            {
                if(y == current_selection)
                {
                    attron(COLOR_PAIR(3));
                }
                mvaddch(y, 0, '|');
                mvaddch(y, interface.width - 1, '|');
                if(y == current_selection)
                {
                    attroff(COLOR_PAIR(3));    // Turn off highlight
                }
            }

            attron(COLOR_PAIR(server_is_on ? 2 : 1));    // Green if ON, Red if OFF
            mvprintw(IP_LINE, 1, "Server IP:  %s", "192.0.0.70");
            attroff(COLOR_PAIR(server_is_on ? 2 : 1));                                                             // Turn off the color
            mvprintw(SERVER_ALIVE_LINE, 1, "%s", server_is_on ? "Server is running" : "Server is not running");    // Change to take connection status check

            if(last_update_time != 0 && server_is_on == 1)
            {
                localtime_r(&last_update_time, &time_info);
                strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &time_info);    // Format as HH:MM:SS
                mvprintw(USER_COUNT_LINE, 1, "%d users    last updated at %s", user_count, time_buffer);
            }
            else if(last_update_time != 0 && server_is_on == 0)
            {
                mvprintw(USER_COUNT_LINE, 1, "%d users    last updated at %s", user_count, time_buffer);
            }
            else
            {
                mvprintw(USER_COUNT_LINE, 1, "%d users    last updated at N/A", 0);
            }
            mvprintw(STARTUP_BUTTON, 1, "Start Server");

            mvprintw(SHUTDOWN_BUTTON, 1, "Shutdown Server");
        }

        refresh();
        nanosleep(&sleep_time, NULL);    // sleep the display function for some time.
    }

    return NULL;
}
