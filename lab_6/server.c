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
    int fd_count = 5;
    
    while(1) {
        testfds = readfds;

        printf("server waiting\n");
        result = select(fd_count, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (result == -1) {
            perror("server");
            exit(1);
        }

        for (int fd = 0; fd < fd_count; fd++) {
            if (FD_ISSET(fd, &testfds)) {
                if (fd == server_sockfd) {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
                    FD_SET(client_sockfd, &readfds);
                    printf("adding client-%d\n", client_sockfd);
                    fd_count++;
                } else {
                    int size;
                    ioctl(fd, FIONREAD, &size);
                    
                    char *msg = malloc(sizeof(char) * size);
                    read(fd, msg, size);

                    if (strcmp(msg, "/exit") == 0) {
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing client-%d\n", fd);
                        fd_count--;
                    } else if (strcmp(msg, "/list") == 0) {
                        // ** server shows to a client a list of clients **
                    } else {
                        printf("client-%d message (%d bytes): %s\n", fd, size, msg);
                        
                        for (int fd_i = 4; fd_i < fd_count; fd_i++) {
                            if (fd_i != fd) {
                                int size1 = size + 15;
                                char *buf = malloc(sizeof(char) * size1);
                                sprintf(buf, "client-%d: %s", fd, msg);
                                write(fd_i, &size1, sizeof(int));
                                write(fd_i, buf, size1);
                                free(buf);
                            }
                        }
                    }

                    free(msg);
                }
            }
        }
    }

    exit(0);
}