#include "../include/handle_client_requests.h"
#include "../include/setup_helper.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080

static void setup_signal_handler(void);
static void sigint_handler(int sig_num);

static volatile sig_atomic_t exit_flag = 0;

int main(void)
{
    int                server_fd;
    socklen_t          server_manager_addr_len;
    struct sockaddr_in server_manager_address;
    pthread_t          server_listener_thread;

    setup_socket(&server_fd);
    setup_address(&server_manager_address, &server_manager_addr_len, PORT);

    if(bind(server_fd, (struct sockaddr *)&server_manager_address, server_manager_addr_len) != 0)
    {
        perror("Bind failed:");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_fd, SOMAXCONN) != 0)
    {
        perror("Listen failed:");
        close(server_fd);
        return EXIT_FAILURE;
    }

    setup_signal_handler();

    while(!exit_flag)
    {
        int client_fd;

        client_fd = accept(server_fd, (struct sockaddr *)&server_manager_address, &server_manager_addr_len);

        if (client_fd < 0)
        {
            if(exit_flag)
            {
                break;
            }
            perror("Client accept failed:");
            continue;
        }

        //Code still to do
    }

    return EXIT_SUCCESS;
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
        perror("Setup signal handler failed:");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void sigint_handler(int sig_num)
{
    const char *shutdown_msg = "Server manager shutting down.\n";
    exit_flag                = 1;
    write(1, shutdown_msg, strlen(shutdown_msg) + 1);
}

#pragma GCC diagnostic pop
