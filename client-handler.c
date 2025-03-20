#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define BUFFER_SIZE 104857600

typedef struct client_opts {
  int* all_clients;
  int total_clients;
  int* client_fd_ref;
  int client_fd;
  int* kill_process;
} client_opts;

void* handle_client(void* opts) {
    client_opts* copts = (struct client_opts*)opts; 
    char buffer[1024];
    char resBuf[1024];
    size_t bytes_received = 1;
    int* client_fd_ref = copts->client_fd_ref;
    int client_fd = copts->client_fd;
    int* kill_process = copts->kill_process;

    // print the incoming messages
    while(bytes_received > 0 && !(*kill_process)){
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if(client_fd < 0){
            break;
        }

        buffer[bytes_received] = '\0';
        printf("%d: %s", client_fd, buffer);

        int resBufSize = sprintf(resBuf, "%d: %s\n", client_fd, buffer);

        for(int i=0; i<copts->total_clients; i++){
            if(copts->all_clients[i] > 0 && copts->all_clients[i] != client_fd){
                write(copts->all_clients[i], resBuf, resBufSize);
            }
        }
    }

    // cleanup
    printf("[DEBUG] client disconnected\n");
    if(client_fd){
        close(client_fd);
        *client_fd_ref = -1;
    }

    printf("[DEBUG] closing the client thread gracefully\n");
    return NULL;
}
