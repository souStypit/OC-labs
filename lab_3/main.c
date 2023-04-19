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
int shell_cd(char **args);
void signal_handler(int sig);

// GLOBAL VARIABLES
pid_t pid = -1;

//----------//----------//----------//----------//----------//
int main(void) {
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
    if (strcmp(args[0], "cd") == 0) return shell_cd(args);
    
    pid = fork();
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
int shell_cd(char **args) {
    if (args[1]) {
        if (chdir(args[1]) != 0) printf("cd");
    } else {
        printf("usage: cd <directory>\n");
    }
    
    return 1;
}
void signal_handler(int sig) {
    printf("Stoping the program with id: %d\n", pid);

    if (pid == -1) kill(0, SIGTERM);
    else kill(pid, sig);
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