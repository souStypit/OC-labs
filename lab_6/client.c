#include <stdio.h> 
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <netinet/in.h> 
#include <sys/ioctl.h> 
#include <arpa/inet.h> 

#define CLIENT_MSG_SIZE 256

char *setString(void);
void errorMessage(int condition, const char *fmt, ...);

int main() {
    int sockfd;
    int len, result;
    struct sockaddr_in address;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9734);
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1) {
        perror("oops: client1");
        exit(1);
    }
    
    printf("<----- CHAT ---->\n");

    int flag = 1;
    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork failed");
        exit(1);
    }

    do {
        if (pid == 0){
            int size, res;
            //ioctl(sockfd, FIONREAD, &size);
            res = read(sockfd, &size, sizeof(int));
            if (res == 0) {
                exit(0);
            }

            char *msg1 = malloc(sizeof(char) * size);
            res = read(sockfd, msg1, size);

            if (res == 0) {
                free(msg1);
                exit(0);
            } else {
                printf("%s\n", msg1);
                free(msg1);
            }
            
        } else {
            char *msg = setString();
            int size = strlen(msg) + 1;

            write(sockfd, msg, size);

            if (strcmp(msg, "/exit") == 0) {
                flag = 0;
                close(sockfd);
                wait(NULL);
            }
            free(msg);
        }
    } while (flag);

    exit(0);
}

void errorMessage(int condition, const char *fmt, ...) {
    //#include <stdarg.h>
    if (condition) {
        va_list argp;
        va_start(argp, fmt);
        vprintf(fmt, argp);
        va_end(argp);
        exit(EXIT_FAILURE);
    }
}
char *setString(void) {
    char *string = malloc(sizeof(char));
    int i = 0;
    char ch;

    while ((ch = getchar()) != '\n') {
        string[i++] = ch;
        void *tmp = realloc(string, sizeof(char) * (i + 1));
        errorMessage(!tmp, "Realloc mistake.\n");
        string = tmp;
    }
    string[i] = '\0';

    // errorMessage(!i, "Empty string.");
    // return string;
    return (i) ? string : NULL;
}