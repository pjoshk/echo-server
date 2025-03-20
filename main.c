#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "client-handler.c"
#include <signal.h>

#define MAX_CONN_ALLOWED 3
// TODO: it doesn't connect more than three even if someone disconnects so we need to fix that.
// the bug is when the client disconnects,
// apart from closing client_fd we need to make the client_fds[j] to -1 as well

int client_fds[MAX_CONN_ALLOWED] = { [0 ... (MAX_CONN_ALLOWED-1)] = -1 };
int server_fd;
int kill_process = 0;

void handle_sigint() {
    kill_process = 1;
    for(int j=0; j<MAX_CONN_ALLOWED; j++){
        close(client_fds[j]);
        client_fds[j] = -1;
    }
    close(server_fd);
}

int main() {
    int PORT = 6969;
    struct sockaddr_in server_addr;

    // intercept user trying to close the server by pressing ^C
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
    if (listen(server_fd, 0) < 0) {
        perror("[DEBUG][ERROR] listen failed\n");
        exit(EXIT_FAILURE);
    }

    printf("[DEBUG] Server listening on port %d\n", PORT);

    while (!kill_process) {
        // keep looping till a client slot is available
        // --------------------------------------------
        int idx = 0;
        while(idx >= 0){
            if(idx == MAX_CONN_ALLOWED){
                idx = 0;
            }
            if(client_fds[idx] < 0){
                break;
            }
            idx++;
        }
        // --------------------------------------------

        int* client_fd_ref;
        int client_fd;

        // client info
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // accept client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if(client_fd < 0){
            perror("[DEBUG][ERROR] accept failed\n");
            continue;
        } else {
            // take the available client slot
            // --------------------------------------------
            for(int j=0; j<MAX_CONN_ALLOWED; j++){
                if(client_fds[j] == -1){
                    client_fds[j] = client_fd;
                    client_fd_ref = &client_fds[j];
                    break;
                }
            }
            // --------------------------------------------
        }

        // offload the client connection to a thread
        // --------------------------------------------
        client_opts copts = {
            .client_fd_ref = client_fd_ref,
            .client_fd = client_fd,
            .kill_process = &kill_process
        };
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &copts);
        printf("[DEBUG] Connected to Client\n");
        // --------------------------------------------
    }

    printf("[DEBUG] closing the main process gracefully\n");
    return 0;
}
