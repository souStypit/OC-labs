#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/ioctl.h> 
#include <netinet/in.h> 

#define CLIENT_MSG_SIZE 256

int main() {
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    int result;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    fd_set readfds, testfds;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, (socklen_t)server_len);

    listen(server_sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);
    
    while(1) {
        char msg[CLIENT_MSG_SIZE];
        int fd;

        testfds = readfds;

        printf("server waiting\n");
        result = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (result < 1/* ??? result != -1 ??? */) {
            perror("server");
            exit(1);
        }

        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &testfds)) {
                if (fd == server_sockfd) {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
                    FD_SET(client_sockfd, &readfds);
                    printf("adding client-%d\n", client_sockfd);
                } else {
                    read(fd, &msg, CLIENT_MSG_SIZE);
                    if (strcmp(msg, "exit") == 0) {
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing client-%d\n", fd);
                    } else {
                        printf("client-%d message: %s\n", fd, msg);
                        write(fd, &msg, CLIENT_MSG_SIZE);
                    }
                }
            }
        }
    }

    exit(0);
}