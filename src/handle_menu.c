//
// Created by brandonr on 12/02/25.
//

#include "handle_menu.h"
#include "handle_server_responses.h"
#include "server_status_flags.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <setup_connections.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#define SERVER_START 1
#define SERVER_STOP 0
#define SERVER_ONLINE 0x0C
// #define SERVER_OFFLINE 0x0D

#define KEY_PRESSED_LINE 16
#define FOURTEEN 14
#define FIFTEEN 15
#define ERROR_LINE 1    // Display any errors on this line
// #define IP_LINE 1              // Display the ip of the connected server
#define SERVER_ALIVE_LINE 2    // Display a message for the alive status of the server
#define USER_COUNT_LINE 3      // Display the user count and the last time it was updated
#define STARTUP_BUTTON 12      // Display the server startup button
#define SHUTDOWN_BUTTON 13     // Display the server shutdown button
#define TIME_BUFFER_SIZE 20    // Time buffer size for formatted local time array

#define SLEEP_LENGTH 50000000L

// if start and no connection display message

static struct menu interface;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
// Server info for fd
static struct server_info server_info;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int end_display(void)
{
    endwin();
    return 1;
}

// Used to know the servers fd
void update_server_connection_info(int server_fd)
{
    server_info.fd = server_fd;
    // server_info.server_on = type;
}

void *handle_input(void *arg)
{
    struct timespec sleep_time;

    (void)arg;
    sleep_time.tv_sec  = 0;
    sleep_time.tv_nsec = SLEEP_LENGTH;

    getmaxyx(stdscr, interface.window_height, interface.window_width);    // Get terminal size
    interface.height                 = FOURTEEN;
    interface.width                  = interface.window_width;
    interface.current_selection      = 1;
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
                        if(atomic_load(&starter_connected_flag))    // Ensure the starter is connected
                        {
                            interface.server_is_on = 1;
                            send_starter_message(server_info.fd, SERVER_START);
                        }
                        else
                        {
                            mvprintw(ERROR_LINE, 1, "Error: Server not connected!");
                            refresh();
                        }
                    }
                    else if(interface.current_selection == SHUTDOWN_BUTTON)
                    {
                        if(atomic_load(&starter_connected_flag))    // Ensure the starter is connected
                        {
                            interface.server_is_on = 0;
                            send_starter_message(server_info.fd, SERVER_STOP);
                        }
                        else
                        {
                            mvprintw(ERROR_LINE, 1, "Error: Server not connected!");
                            refresh();
                        }
                    }
                    refresh();
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
        // attron(COLOR_PAIR(interface.server_is_on ? 2 : 1));    // Green if ON, Red if OFF
        // mvprintw(IP_LINE, 1, "Server IP:  %s", interface.ip_address);
        // attroff(COLOR_PAIR(interface.server_is_on ? 2 : 1));    // Turn off color

        // Server Status
        attron(COLOR_PAIR(interface.server_is_on == SERVER_ONLINE ? 2 : 1));    // Green if ON, Red if OFF
        mvprintw(SERVER_ALIVE_LINE, 1, "%s", interface.server_is_on == SERVER_ONLINE ? "Starter is running" : "Starter is not running");
        attroff(COLOR_PAIR(interface.server_is_on == SERVER_ONLINE ? 2 : 1));    // Green if ON, Red if OFF

        // Display Last Updated Time
        if(interface.last_updated_time > 0)
        {
            localtime_r(&interface.last_updated_time, &time_info);
            strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &time_info);    // Format as HH:MM:SS
            mvprintw(USER_COUNT_LINE, 1, "%d users   %d messages last updated at %s", interface.user_count, interface.message_count, time_buffer);
        }
        else
        {
            mvprintw(USER_COUNT_LINE, 1, "%d users  %d messages  last updated at N/A", interface.user_count, interface.message_count);
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
    if(interface.server_is_on == SERVER_ONLINE)
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

void update_server_user_count(const int user_count)
{
    // This check just ensures with the current setup, that the server is "connected/on" so it isn't printing data of a non-existent server.
    if(interface.server_is_on == SERVER_ONLINE)
    {
        interface.user_count = user_count;
    }
}

void update_server_message_count(const int message_count)
{
    // This check just ensures with the current setup, that the server is "connected/on" so it isn't printing data of a non-existent server.
    if(interface.server_is_on == SERVER_ONLINE)
    {
        interface.message_count = message_count;
    }
}

void update_server_status(const uint8_t status)
{
    interface.server_is_on = status;
}
