#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "game.h" // Include this if your game functions are in game.c

#define SERVER_IP "127.0.0.1" // replace with your server IP
#define SERVER_PORT 2000 // replace with your server port

void *handle_client(void *arg) {
    int connfd = *((int *)arg);
    free(arg); // Don't forget to free the memory!

    // Here you can call the functions from game.c as needed
    // For example, if the client sends a request to join a room:
    int player_index = ...; // Get the player index
    join_room(player_index, connfd);

    // Or if the client sends a request to create a room:
    create_room(player_index, connfd);

    // Or if the client sends a request to start a quiz:
    int categoryIndex = ...; // Get the category index
    quiz(categoryIndex, connfd);

    close(connfd);
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a port
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Accept connections and handle them in separate threads
    while (1) {
        len = sizeof(cli);
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0) {
            perror("Server accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, &connfd) != 0) {
            perror("Failed to create thread");
            close(connfd);
        }
    }

    close(sockfd);

    return 0;
}
