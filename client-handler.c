#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define BUFFER_SIZE 104857600

typedef struct client_opts {
  int* server_fd;
  int* client_fd;
  int* kill_process;
} client_opts;

void* handle_client(void* opts) {
    client_opts* copts = (struct client_opts*)opts; 
    char buffer[1024];
    size_t bytes_received = 1;
    int* server_fd = copts->server_fd;
    int* client_fd = copts->client_fd;
    int* kill_process = copts->kill_process;

    // print the incoming messages
    while(bytes_received > 0 && !(*kill_process)){
        bytes_received = recv(*client_fd, buffer, BUFFER_SIZE, 0);
        if(*client_fd < 0){
            break;
        }

        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }

    // cleanup
    printf("[DEBUG] client disconnected\n");
    if(*client_fd){
        close(*client_fd);
    }

    return NULL;
}
