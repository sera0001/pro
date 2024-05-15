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

    int choice;
    do {
        char menuMessage[] = "\n--- Menu ---\n1. Signup\n2. Login\n3. Play as a guest\n4. Exit\nEnter your choice: ";
        send(connfd, menuMessage, sizeof(menuMessage), 0);

        int bytes_received = recv(connfd, &choice, sizeof(choice), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break; // Exit the loop if client disconnected
        }

        switch (choice) {
            case 1:
                printf("Client chose to signup.\n");
                signup(connfd);
                break;
            case 2:
                printf("Client chose to login.\n");
                login(connfd);
                break;
            case 3:
                printf("Client chose to play as a guest.\n");
                // The server should send game data after the connection is established
                char game_data[1024];
                recv(connfd, game_data, sizeof(game_data), 0);

                // Start your game here using the game_data
                startGame(connfd);
                break;
            case 4:
                printf("Client chose to exit.\n");
                char exitMessage[] = "Exiting program...\n";
                send(connfd, exitMessage, sizeof(exitMessage), 0);
                break;
            default:
                printf("Client chose an invalid option.\n");
                char invalidChoiceMessage[] = "Invalid choice. Please enter a number from 1 to 4.\n";
                send(connfd, invalidChoiceMessage, sizeof(invalidChoiceMessage), 0);
        }
    } while (choice != 4);

    close(connfd);
    return NULL;
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
        int *new_sock = malloc(sizeof(int));
        *new_sock = connfd;
        if (pthread_create(&tid, NULL, handle_client, new_sock) != 0) {
            perror("Failed to create thread");
            close(connfd);
        }
    }

    close(sockfd);

    return 0;
}
