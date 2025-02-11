#include "../include/handle_client_requests.h"
#include "../include/setup_helper.h"
#include <arpa/inet.h>
#include <errno.h>
#include <handle_server_responses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define CLIENT_PORT 8080
// #define STARTER_PORT 8090
#define SERVER_PORT 9000

static void setup_signal_handler(void);
static void sigint_handler(int sig_num);

static volatile sig_atomic_t exit_flag = 0;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main(void)
{
    // int client_fd;
    // int starter_fd;
    int server_fd;

    // socklen_t          client_addr_len;
    // struct sockaddr_in client_address;
    // pthread_t          client_listener_thread;

    // socklen_t          starter_addr_len;
    // struct sockaddr_in starter_address;
    // pthread_t          starter_listener_thread;

    socklen_t          server_addr_len;
    struct sockaddr_in server_address;
    pthread_t          server_listener_thread;

    // setup_socket(&client_fd);
    // setup_address(&client_address, &client_addr_len, CLIENT_PORT);

    // setup_socket(&starter_fd);
    // setup_address(&starter_address, &starter_addr_len,SERVER_PORT);

    setup_socket(&server_fd);
    setup_address(&server_address, &server_addr_len, SERVER_PORT);

    // if(bind(client_fd, (struct sockaddr *)&client_address, client_addr_len) != 0)
    // {
    //     perror("Client bind failed");
    //     printf("Error: %d\n", errno);
    //     close(client_fd);
    //     return EXIT_FAILURE;
    // }
    //
    // if(listen(client_fd, SOMAXCONN) != 0)
    // {
    //     perror("Listen failed");
    //     printf("Error code: %d\n", errno);
    //     close(client_fd);
    //     return EXIT_FAILURE;
    // }

    // if(bind(starter_fd, (struct sockaddr *)&starter_address, starter_addr_len) != 0)
    // {
    //     perror("Starter bind failed");
    //     printf("Error: %d\n", errno);
    //     close(starter_fd);
    //     return EXIT_FAILURE;
    // }
    //
    // if(listen(starter_fd, SOMAXCONN) != 0)
    // {
    //     perror("Starter listen failed");
    //     printf("Error code: %d\n", errno);
    //     close(starter_fd);
    //     return EXIT_FAILURE;
    // }

    if(bind(server_fd, (struct sockaddr *)&server_address, server_addr_len) != 0)
    {
        perror("Server bind failed");
        printf("Error: %d\n", errno);
        close(server_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_fd, SOMAXCONN) != 0)
    {
        perror("Server listen failed");
        printf("Error code: %d\n", errno);
        close(server_fd);
        return EXIT_FAILURE;
    }

    setup_signal_handler();

    while(!exit_flag)
    {
        // int *client_requests_fd_ptr;
        // int  client_requests_fd;

        // int *ss_fd_ptr;
        // int  ss_fd;

        int *running_server_fd_ptr;
        int  running_server_fd;

        // client_requests_fd = accept(client_fd, (struct sockaddr *)&client_address, &client_addr_len);
        //
        // client_requests_fd_ptr = (int *)malloc(sizeof(int));
        //
        // if(client_requests_fd < 0)
        // {
        //     if(exit_flag)
        //     {
        //         break;
        //     }
        //     perror("Client accept failed");
        //     continue;
        // }

        running_server_fd = accept(server_fd, (struct sockaddr *)&server_address, &server_addr_len);

        running_server_fd_ptr = (int *)malloc(sizeof(int));

        if(running_server_fd < 0)
        {
            if(exit_flag)
            {
                free(running_server_fd_ptr);
                break;
            }
            perror("Client accept failed");
            continue;
        }

        printf("Accepted connection on server_fd: %d\n", running_server_fd);

        // ss_fd_ptr     = (int *)malloc(sizeof(int));
        // ss_fd = accept(starter_fd, (struct sockaddr *)&starter_address, &starter_addr_len);

        // if(ss_fd < 0)
        // {
        //     if(exit_flag)
        //     {
        //         free(ss_fd_ptr);
        //         break;
        //     }
        //     perror("Server starter accept failed");
        //     free(ss_fd_ptr);
        //     continue;
        // }

        // *client_requests_fd_ptr = client_requests_fd;
        // *ss_fd_ptr = server_fd;
        *running_server_fd_ptr = running_server_fd;
        //
        // if(pthread_create(&client_listener_thread, NULL, handle_client, (void *)client_requests_fd_ptr) != 0)
        // {
        //     perror("Client requests thread creation failed");
        //     close(client_requests_fd);
        //     free(client_requests_fd_ptr);
        // }

        // if(pthread_create(&starter_listener_thread, NULL, handle_starter, (void *)ss_fd_ptr) != 0)
        // {
        //     perror("Starter listener thread creation failed");
        //     free(ss_fd_ptr);
        // }

        if(pthread_create(&server_listener_thread, NULL, handle_server_response, (void *)running_server_fd_ptr) != 0)
        {
            perror("Server listener thread creation failed");
            close(running_server_fd);
            free(running_server_fd_ptr);
        }

        // pthread_detach(client_listener_thread);
        // pthread_detach(starter_listener_thread);
        pthread_detach(server_listener_thread);
    }
    // close(client_fd);
    close(server_fd);
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
    exit_flag                = 1;
    write(1, shutdown_msg, strlen(shutdown_msg) + 1);
}

#pragma GCC diagnostic pop
