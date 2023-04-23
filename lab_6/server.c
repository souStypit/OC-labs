#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/ioctl.h> 
#include <netinet/in.h>

#define INIT_CLIENT 4

int main() {
    int server_sockfd, client_sockfd, server_len, client_len;
    int result;
    struct sockaddr_in server_address, client_address;
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
    
    int fd_count = INIT_CLIENT;
    while(1) {
        testfds = readfds;

        result = select(fd_count, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (result == -1) {
            perror("Server");
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
                        printf("removing client-%d\n", fd);
                        
                        close(fd);
                        FD_CLR(fd, &readfds);
                        fd_count--;
                    } else if (strcmp(msg, "/list") == 0) {
                        printf("sending list to client-%d\n", fd);
                        int code = 1;
                        write(fd, &code, sizeof(int));
                        int amount_of_users = fd_count - INIT_CLIENT;
                        write(fd, &amount_of_users, sizeof(int));
                        for (int i = INIT_CLIENT; i < fd_count; i++) {
                            write(fd, &i, sizeof(int));
                        }
                    } else {
                        printf("client-%d says: %s\n", fd, msg);
                        
                        for (int fd_i = INIT_CLIENT; fd_i < fd_count; fd_i++) {
                            if (fd_i != fd) {
                                int code = 0;
                                write(fd_i, &code, sizeof(int));
                                int size1 = size + 15;
                                write(fd_i, &size1, sizeof(int));
                                char *buf = malloc(sizeof(char) * size1);
                                sprintf(buf, "client-%d: %s", fd, msg);
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
