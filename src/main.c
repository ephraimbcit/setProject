#include "../include/handle_client_requests.h"
#include "../include/setup_helper.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080

int main(void)
{
    int                server_fd;
    socklen_t          server_manager_addr_len;
    struct sockaddr_in server_manager_address;

    setup_socket(&server_fd);
    setup_address(&server_manager_address, &server_manager_addr_len, PORT);
    handle_client();

    return EXIT_SUCCESS;
}
