#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h> 
#include <unistd.h> 

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

    while (1) {
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
                    char *msg = "HELLO\n";
                    int size = strlen(msg);
                    write(fd, &size, sizeof(int));
                    write(fd, msg, size);
                }
            }
        }
    }
}