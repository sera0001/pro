#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1" // replace with your server IP
#define SERVER_PORT 2000 // replace with your server port

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Server connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server at %s:%d\n", SERVER_IP, SERVER_PORT);

   int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Play as a guest\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Send choice to server as text
        char choice_str[10];
        sprintf(choice_str, "%d", choice);
        send(sockfd, choice_str, strlen(choice_str) + 1, 0);

        // Now let's handle the server's response
        char server_response[1024]; // Adjust the size as needed
        int bytes_received = recv(sockfd, server_response, sizeof(server_response) - 1, 0);

        if (bytes_received < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }

        // Null-terminate the string
        server_response[bytes_received] = '\0';

        printf("Server says: %s\n", server_response);

    } while (choice != 4);

    close(sockfd);

    return 0;
}
