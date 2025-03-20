//
// Created by brandonr on 12/02/25.
//

#include "handle_menu.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <setup_connections.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

// #define FIVE 5
#define THREE 3
// #define SIX 6
// #define SEVEN 7
// #define TEN 10
// #define TWELVE 12
// #define THIRTEEN 13
#define HUNDRED 100
#define MAX_IP 256
#define PREFIX 192

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

static struct menu interface;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

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
    // struct menu     interface;

    // int             height_too_short  = 0;
    // time_t          last_update_time  = 0;

    // int         user_count   = 0;
    // const char *connected_ip = "192.0.0.70";

    (void)arg;
    sleep_time.tv_sec  = 0;
    sleep_time.tv_nsec = SLEEP_LENGTH;

    getmaxyx(stdscr, interface.window_height, interface.window_width);    // Get terminal size
    interface.height                 = FOURTEEN;
    interface.width                  = interface.window_width;
    interface.current_selection      = 1;
    interface.server_is_on           = 0;    // 1 is on, 0 is off
    interface.last_updated_time      = 0;
    interface.vertical_border_char   = '|';
    interface.horizontal_border_char = '-';
    interface.user_count             = 0;

    while(!exit_flag)    // Keep handling input until exit
    {
        // int new_window_height;
        // int new_window_width;
        int ch = getch();

        // pthread_mutex_lock(&menu_mutex);

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
                    if((interface.current_selection - 1) != 0)
                    {
                        interface.current_selection--;
                    }
                    refresh();
                    break;
                case KEY_DOWN:
                    // mvprintw(SIX, 1, "Down key pressed!");
                    if((interface.current_selection + 1) != FOURTEEN)
                    {
                        interface.current_selection++;
                    }
                    refresh();
                    break;
                case '\n':
                    // mvprintw(SIX, 1, "Down key pressed!");
                    if(interface.current_selection == STARTUP_BUTTON)
                    {
                        interface.server_is_on = 1;
                    }
                    else if(interface.current_selection == SHUTDOWN_BUTTON)
                    {
                        interface.server_is_on = 0;
                    }
                    refresh();
                    break;
                case 'k':    // Get current time when 'k' is pressed
                    if(interface.server_is_on == 1)
                    {
                        interface.user_count        = rand() % MAX_USERS;
                        interface.last_updated_time = time(NULL);
                    }
                    break;
                case 'p':    // Get current time when 'k' is pressed
                    interface.server_is_on = !interface.server_is_on;
                    break;
                default:
                    mvprintw(KEY_PRESSED_LINE, 1, "Pressed: %c ", ch);
                    refresh();
            }
        }
        // pthread_mutex_unlock(&menu_mutex);
        handle_display();
        nanosleep(&sleep_time, NULL);    // sleep the display function for some time.
    }

    return NULL;
}

// void handle_display(const char *ip_address, int user_count, struct menu *interface)
void handle_display(void)
{
    char      time_buffer[TIME_BUFFER_SIZE];    // Buffer to store formatted time
    struct tm time_info;

    // pthread_mutex_lock(&menu_mutex);

    clear();
    mvprintw(FIFTEEN, 1, "Current selection: %d ", interface.current_selection);

    // Draw Borders
    for(int x = 0; x < interface.width; x++)
    {
        mvaddch(0, x, interface.horizontal_border_char);
        mvaddch(interface.height, x, interface.horizontal_border_char);
    }
    for(int y = 0; y <= interface.height; y++)
    {
        if(y != interface.height && y != 0)
        {
            if(y == interface.current_selection)
            {
                attron(COLOR_PAIR(3));
            }
            mvaddch(y, 0, interface.vertical_border_char);
            mvaddch(y, interface.width - 1, interface.vertical_border_char);
            if(y == interface.current_selection)
            {
                attroff(COLOR_PAIR(3));    // Turn off highlight
            }
        }

        // Display IP Address
        attron(COLOR_PAIR(interface.server_is_on ? 2 : 1));    // Green if ON, Red if OFF
        mvprintw(IP_LINE, 1, "Server IP:  %s", interface.ip_address);
        attroff(COLOR_PAIR(interface.server_is_on ? 2 : 1));    // Turn off color

        // Server Status
        mvprintw(SERVER_ALIVE_LINE, 1, "%s", interface.server_is_on ? "Server is running" : "Server is not running");

        // Display Last Updated Time
        if(interface.last_updated_time > 0)
        {
            localtime_r(&interface.last_updated_time, &time_info);
            strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &time_info);    // Format as HH:MM:SS
            mvprintw(USER_COUNT_LINE, 1, "%d users    last updated at %s", interface.user_count, time_buffer);
        }
        else
        {
            mvprintw(USER_COUNT_LINE, 1, "%d users    last updated at N/A", interface.user_count);
        }

        // Menu Options
        mvprintw(STARTUP_BUTTON, 1, "Start Server");
        mvprintw(SHUTDOWN_BUTTON, 1, "Shutdown Server");
    }

    // pthread_mutex_unlock(&menu_mutex);
    refresh();
}

void update_info(const char *ip_address, int user_count)
{
    if(interface.server_is_on)
    {
        // pthread_mutex_lock(&menu_mutex);

        // Store IP Address
        strncpy(interface.ip_address, ip_address, INET_ADDRSTRLEN - 1);
        interface.ip_address[INET_ADDRSTRLEN - 1] = '\0';    // Ensure null termination

        // Store User Count
        interface.user_count = user_count;

        // Store Last Updated Time
        interface.last_updated_time = time(NULL);    // Store raw timestamp

        // pthread_mutex_unlock(&menu_mutex);
    }
}

void *test_update_info(void *arg)
{
    // char random_ip[INET_ADDRSTRLEN];
    const char static_ip[INET_ADDRSTRLEN] = "192.168.1.100";    // Static IP address

    // srand((unsigned int)time(NULL));    // Fix: Explicit cast

    (void)arg;
    while(!exit_flag)
    {
        int sleep_time;
        int random_user_count = rand() % HUNDRED;    // Generate random user count (0-99)

        // generate_random_ip(random_ip, sizeof(random_ip));    // Generate random IP

        // pthread_mutex_lock(&menu_mutex);
        // update_info(random_ip, random_user_count);
        update_info(static_ip, random_user_count);
        // pthread_mutex_unlock(&menu_mutex);

        sleep_time = THREE + rand() % THREE;    // Fix: Move declaration
        sleep((unsigned int)sleep_time);        // Fix: Explicit cast
    }

    return NULL;
}

void generate_random_ip(char *buffer, size_t size)
{
    snprintf(buffer, size, "%d.%d.%d.%d", PREFIX, rand() % MAX_IP, rand() % MAX_IP, rand() % MAX_IP);
}
