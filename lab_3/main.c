#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE 10

int stack[STACK_SIZE];
int top = 0;

void push(int data) { 
    if (top < STACK_SIZE) { 
        stack[top++] = data; 
    } 
}
int pop() { 
    return (top > 0) ? stack[--top] : -1; 
}

// TEMPLATE FUNCTIONS
char **parseString(char *line);
char *setString(void);
void errorMessage(int condition, const char *message, ...);

// FUNCTIONS
int shell(char **args);
int shell_cd(char **args);
void signal_handler(int sig);

// GLOBAL VARIABLES

//----------//----------//----------//----------//----------//
int main(void) {
    signal(SIGINT, signal_handler);

    int status = 1;
    do {
        printf("> ");

        char **args = parseString(setString());
        if (args) status = shell(args);

        free(args);
        printf("\n");
    } while (status);

    exit(EXIT_SUCCESS);
}
//----------//----------//----------//----------//----------//

// FUNCTION IMPLEMETATION
int shell(char **args) {
    if (strcmp(args[0], "exit") == 0) return 0;
    if (strcmp(args[0], "cd") == 0) return shell_cd(args);

    int pid = fork();
    push(pid);
    switch (pid) {
        case -1:
            printf("Fork failed");
            exit(1);
        case 0:
            execvp(args[0], args);
            exit(0);
        default:
            waitpid(pid, NULL, 0);
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
    int pid = pop();

    if (pid != -1) {
        printf("Stoping the process with id: %d\n", pid);
        kill(pid, sig);
    } else {
        printf("No more processes to kill\n");
        exit(0);
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
    if (i) return string;

    free(string);
    return NULL;
}