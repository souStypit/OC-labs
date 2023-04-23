#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void *receive_msg(void *arg);
void *send_msg(void *arg);

char *setString(void);
void errorMessage(int condition, const char *fmt, ...);

int main() {
    int sockfd;
    int len, result;
    struct sockaddr_in address;
    pthread_t thread_receive, thread_send;

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
    pthread_create(&thread_send, NULL, send_msg, &sockfd);

    pthread_join(thread_receive, NULL);
    pthread_join(thread_send, NULL);

    close(sockfd);
    exit(0);
}
void *receive_msg(void *arg) {
    int sockfd = *(int *)arg;
    while (1) {
        int size, res, code;
        res = read(sockfd, &code, sizeof(int));
        if (res == -1) {
            perror("Receiving code error");
            exit(1);
        }
        if (res == 0) {
            printf("Server disconnection.\n");
            exit(0);
        }

        if (code == 0) {
            res = read(sockfd, &size, sizeof(int));
            if (res == -1) {
                perror("Receiving size error");
                exit(1);
            }
            if (res == 0) {
                printf("Server disconnection.\n");
                exit(0);
            }

            char *msg1 = malloc(sizeof(char) * size);
            res = read(sockfd, msg1, size);
            if (res == -1) {
                perror("Receiving message error");
                exit(1);
            }
            if (res == 0) {
                printf("Server disconnection.\n");
                free(msg1);
                exit(0);
            } else {
                printf("%s\n", msg1);
                free(msg1);
            }
        }
        if (code == 1) {
            int count, number;
            read(sockfd, &count, sizeof(int));
            printf(" List of members (Total: %d):\n", count);
            for (int i = 0; i < count; i++) {
                read(sockfd, &number, sizeof(int));
                printf("  client-%d\n", number);
            }
            printf("___________\n");
        }
    }
}
void *send_msg(void *arg) {
    int sockfd = *(int *)arg;
    while (1) {
        char *msg = setString();
        if (msg) {
            int size = strlen(msg) + 1;
            int res = write(sockfd, msg, size);

            if (res == -1) {
                perror("Send error");
                exit(1);
            }

            if (res == 0 || strcmp(msg, "/exit") == 0) {
                exit(0);
            }

            free(msg);
        }
    }
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
    if (i) return string;

    free(string);
    return NULL;
}