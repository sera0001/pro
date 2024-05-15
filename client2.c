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

int connect_to_server() {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}
int main() {
    int sockfd = connect_to_server();

    if (sockfd < 0) {
        printf("Failed to connect to server.\n");
        return EXIT_FAILURE;
    }

    printf("Connected to server.\n");

    // Add your client code here...

    close(sockfd);
    return EXIT_SUCCESS;
}

