#include "../include/handle_client_requests.h"
#include "../include/setup_connections.h"
#include "../include/setup_helper.h"
#include "../include/setup_menu.h"
#include "../include/handle_menu.h"
#include <arpa/inet.h>
#include <errno.h>
#include <handle_server_responses.h>
#include <pthread.h>
#include <setup_listener.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define TYPE_STARTER 0
#define TYPE_SERVER 1
#define TYPE_CLIENT 2

static void setup_signal_handler(void);
static void sigint_handler(int sig_num);

volatile sig_atomic_t exit_flag = 0;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(void)
{
    // int starter_fd;
    int server_fd;
    int client_fd;

    // int starter_listener_successful;
    int server_listener_successful;
    int client_listener_successful;

    // pthread_t          starter_connections_thread;
    pthread_t server_connections_thread;
    pthread_t client_connections_thread;

    struct connection_info *client_connection_info;
    struct connection_info *server_connection_info;

//---------------- start ncurses display.
    // start_display();
//----------------
    // starter_listener_successful = setup_listener(&starter_fd, TYPE_STARTER);
    server_listener_successful = setup_listener(&server_fd, TYPE_SERVER);
    client_listener_successful = setup_listener(&client_fd, TYPE_CLIENT);

    if(server_listener_successful == EXIT_FAILURE)
    {
        printf("Failed to setup server listener\n");
        return EXIT_FAILURE;
    }

    if(client_listener_successful == EXIT_FAILURE)
    {
        printf("Failed to setup client listener\n");
        return EXIT_FAILURE;
    }

    setup_signal_handler();

    server_connection_info = malloc(sizeof(struct connection_info));

    if(!server_connection_info)
    {
        perror("Memory allocation failed for server info");
        return EXIT_FAILURE;
    }

    server_connection_info->fd   = server_fd;
    server_connection_info->type = TYPE_SERVER;

    if(pthread_create(&server_connections_thread, NULL, setup_connections, (void *)server_connection_info) != 0)
    {
        perror("Failed to create server thread");
        free(server_connection_info);    // Free on failure
        return EXIT_FAILURE;
    }

    client_connection_info = malloc(sizeof(struct connection_info));

    if(!client_connection_info)
    {
        perror("Memory allocation failed for client info");
        return EXIT_FAILURE;
    }

    client_connection_info->fd   = client_fd;
    client_connection_info->type = TYPE_CLIENT;

    // Create a thread for client connections
    if(pthread_create(&client_connections_thread, NULL, setup_connections, (void *)client_connection_info) != 0)
    {
        perror("Failed to create client thread");
        free(client_connection_info);    // Free on failure
        return EXIT_FAILURE;
    }

    pthread_detach(server_connections_thread);
    pthread_detach(client_connections_thread);

    while(!exit_flag)
    {
        sleep(1);
    }

    close(server_fd);
    close(client_fd);
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
    const char *shutdown_msg = "\nServer manager shutting down.\n";
    // End the ncurses display
    // end_display();
    exit_flag                = 1;
    write(1, shutdown_msg, strlen(shutdown_msg) + 1);
}

#pragma GCC diagnostic pop
