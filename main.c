#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "client-handler.c"

int server_fd;
int client_fd;
int kill_process = 0;

void handle_sigint() {
    kill_process = 1;
    close(client_fd);
    close(server_fd);
}

int main() {
    int PORT = 6969;
    struct sockaddr_in server_addr;

    signal(SIGINT, handle_sigint);

    // create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[DEBUG][ERROR] socket failed\n");
        exit(EXIT_FAILURE);
    }

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

    while (!kill_process) {
        // client info
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // accept client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(client_fd < 0){
            perror("[DEBUG][ERROR] accept failed\n");
            close(client_fd);
            continue;
        }

        printf("[DEBUG] Connected to Client\n");
        handle_client(client_fd, &kill_process);
    }

    printf("[DEBUG] closing gracefully\n");
    return 0;
}
