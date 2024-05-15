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

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        exit(EXIT_FAILURE);
    }

    int choice;
    do {
        char menuMessage[1024];
        recv(sockfd, menuMessage, sizeof(menuMessage), 0);
        printf("%s", menuMessage);

        scanf("%d", &choice);
        send(sockfd, &choice, sizeof(choice), 0);

        switch (choice) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                char game_data[1024];
                recv(sockfd, game_data, sizeof(game_data), 0);
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
