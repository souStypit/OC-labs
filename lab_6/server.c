#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <netinet/in.h> 

#define CLIENT_MSG_SIZE 256

int main() {
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    listen(server_sockfd, 5);

    while(1) {
        char msg[CLIENT_MSG_SIZE];

        printf("server waiting\n");

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);

        read(client_sockfd, &msg, CLIENT_MSG_SIZE);
        write(client_sockfd, &msg, CLIENT_MSG_SIZE);
        printf("adding client on fd %d\n", client_sockfd);
        close(client_sockfd);
    }


    exit(0);
}