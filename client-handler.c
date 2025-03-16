#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#define BUFFER_SIZE 104857600

void* handle_client(int client_fd) {
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    size_t bytes_received = 1;

    // print the incoming messages
    while(bytes_received > 0){
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);

        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    // cleanup
    free(buffer);
    close(client_fd);

    return NULL;
}
