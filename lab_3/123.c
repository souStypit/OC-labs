#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 1024

pid_t pid = -1;

void handle_sigint(int sig) {
    char choice;
    if (pid != -1) {
        printf("\nВыполняется процесс с идентификатором %d. Хотите его завершить? (y/n) ", pid);
        fflush(stdout);
        scanf("%c", &choice);
        if (choice == 'y' || choice == 'Y') {
            kill(pid, sig);
            pid = -1;
        } else {
            printf("Процесс не будет завершен\n");
        }

        while ((getchar()) != '\n')
            ;
    }
}

void execute(char* command, char** arguments) {
    pid = fork();
    if (pid == 0) {
        execvp(command, arguments);
        printf("\n");
        exit(0);
    } else {
        wait(NULL);
        pid = -1;
    }
    printf("\n");
}

int main() {
    char input[MAX_LENGTH];
    char *argv[MAX_LENGTH / 2];
    int flag = 1;

    signal(SIGINT, handle_sigint);

    while (flag) {
        printf(">>> ");
        fflush(stdout);

        fgets(input, MAX_LENGTH, stdin);

        char *token = strtok(input, " \n");
        int i = 0;
        for (i = 0; token != NULL; i++) {
            argv[i] = token;
            token = strtok(NULL, " \n");
        }
        argv[i] = NULL;

        execute(argv[0], argv);
        // if (strcmp(argv[0], "ls") == 0) {
        //     execute("ls", argv);
        // } else if (strcmp(argv[0], ) == 0) {
        //     execute("cat", argv);
        // } else if ((strcmp(argv[0], "firefox") == 0)) {
        //     execute("firefox", argv);
        // } else if (strcmp(argv[0], "exit") == 0) {
        //     flag = 0;
        // } else {
        //     printf("Неизвестная команда\n");
        // }
    }
}
