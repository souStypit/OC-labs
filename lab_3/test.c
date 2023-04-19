#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// TEMPLATE FUNCTIONS
char **parseString(char *line);
char *setString(void);
void errorMessage(int condition, const char *message, ...);

// FUNCTIONS
int shell(char **args);

void signal_handler();

//----------//----------//----------//----------//----------//
int main(void) {
    // signal(SIGINT, SIG_DFL);
    signal(SIGINT, signal_handler);

    int status = 1;
    do {
        printf("> ");

        char **args = parseString(setString());
        if (args) status = shell(args);

        printf("\n");
    } while (status);

    exit(EXIT_SUCCESS);
}
//----------//----------//----------//----------//----------//

// FUNCTION IMPLEMETATION
int shell(char **args) {
    if (strcmp(args[0], "exit") == 0) return 0;
    pid_t pid = fork();

    switch (pid) {
        case -1:
            printf("Fork failed");
            exit(1);
        case 0:
            execvp(args[0], args);
            exit(0);
        default:
            wait(NULL);
            pid = -1;
    }

    return 1;
}
void signal_handler() {
    pid_t pid;
    printf("Type process id that you want to kill (-1 to cancel, 0 to kill current process): ");
    fflush(stdout);
    scanf("%d\n", &pid);

    if (pid != -1) {
        kill(pid, SIGTERM);
        return;
    }

    printf("Terminating is cancelled.");
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
char **parseString(char *line) {
    if (line == NULL) return NULL;
    
    int i = 0;
    char *delim = " \t\n\r", *token = strtok(line, delim);
    char **tokens = malloc(sizeof(char *));
    errorMessage(!tokens, "Malloc mistake.\n");

    while (token != NULL) {
        tokens[i++] = token;
        void *tmp = realloc(tokens, (i + 1) * sizeof(char *));
        errorMessage(!tokens, "Realloc mistake.\n");
        tokens = tmp;

        token = strtok(NULL, delim);
    }

    tokens[i] = NULL;
    return tokens;
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