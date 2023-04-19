#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

#define CLIENT_MSG_SIZE 256

int main() {
    int sockfd;
    int len, result;
    struct sockaddr_in address;
    char msg[CLIENT_MSG_SIZE];
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9734);
    len = sizeof(address);

    printf("Write a message: ");
    scanf("%s", msg);

    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1) {
        perror("oops: client1");
        exit(1);
    }

    write(sockfd, &msg, CLIENT_MSG_SIZE);
    read(sockfd, &msg, CLIENT_MSG_SIZE);
    printf("msg from server = %s\n", msg);
    close(sockfd);
    exit(0);

}