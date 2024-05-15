#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 2000

void signup(int sockfd) ;

void startGame(int sockfd) ;

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Client menu
    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Play as a guest\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Send choice to server
        write(sockfd, &choice, sizeof(choice));

        switch (choice) {
            case 1:
                signup(sockfd);
                break;
            case 2:
                // Implement login functionality
                break;
            case 3:
                // Request game data from the server
                char game_data[1024];
                read(sockfd, game_data, sizeof(game_data));

                // Start game with received game data
                startGame(sockfd);
                break;
            case 4:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number from 1 to 4.\n");
        }
    } while (choice != 4);

    close(sockfd);

    return 0;
}
