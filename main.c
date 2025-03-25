#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "client-handler.c"

int main() {
    int PORT = 6969;
    int server_fd;
    struct sockaddr_in server_addr;

    // create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[DEBUG][ERROR] socket failed\n");
        exit(EXIT_FAILURE);
    }

    printf("[DEBUG] server_fd: %d\n", server_fd);

    // config socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket to port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[DEBUG][ERROR] bind failed\n");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_fd, 10) < 0) {
        perror("[DEBUG][ERROR] listen failed\n");
        exit(EXIT_FAILURE);
    }

    printf("[DEBUG] Server listening on port %d\n", PORT);

    while (1) {
        // client info
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd;

        // accept client connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("[DEBUG][ERROR] accept failed\n");
            continue;
        }

        printf("[DEBUG] client_fd: %d\n", client_fd);
        printf("[DEBUG] Connected to Client\n");

        handle_client(client_fd);
    }

    // cleanup
    printf("[DEBUG] closing server_fd\n");
    close(server_fd);
    return 0;
}
