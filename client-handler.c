#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#define BUFFER_SIZE 104857600

void* handle_client(int client_fd, int* kill_process) {
    // char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    char buffer[1024];
    size_t bytes_received = 1;

    // print the incoming messages
    while(bytes_received > 0 && !(*kill_process)){
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if(client_fd < 0){
            break;
        }

        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }

    // cleanup
    printf("[DEBUG] client disconnected\n");
    if(client_fd){
        close(client_fd);
    }

    return NULL;
}
