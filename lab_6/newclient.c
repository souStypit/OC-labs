#include <stdio.h> 
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <netinet/in.h> 
#include <pthread.h>
#include <arpa/inet.h> 
#include <sys/ioctl.h> 
#include <unistd.h> 

void *receive_msg(void *arg);
void *send_msg(void *arg);

char *setString(void);
void errorMessage(int condition, const char *fmt, ...);

int main() {
    int sockfd;
    int len, result;
    struct sockaddr_in address;
    pthread_t thread_receive;//, thread_send;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9734);
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr *)&address, (socklen_t)len);
    if (result == -1) {
        perror("Client error");
        exit(1);
    }
    
    printf("<----- CHAT ---->\n\n");

    pthread_create(&thread_receive, NULL, receive_msg, &sockfd);
    //pthread_create(&thread_send, NULL, send_msg, &sockfd);

    pthread_join(thread_receive, NULL);
    //pthread_join(thread_send, NULL);

    close(sockfd);
    exit(0);
}

void *receive_msg(void *arg) {
    int sockfd = *(int *)arg;
        char msg[256];
        int size;
        read(sockfd, &size, sizeof(int));
        printf("%d", size);
        read(sockfd, msg, size);
        printf("str: %s\n", msg);
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