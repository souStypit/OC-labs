#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char **parseString(char *line);
char *setString();
void errorMessage(int condition, const char *message);
int sh_launch(char **args);
void onCtrlC();

int sh_cd(char **args);

int main() {
    int status;
    do {
        char **args = parseString(setString());
        status = sh_launch(args);
    } while (status);

    exit(EXIT_SUCCESS);
}

char *setString() {
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

    errorMessage(!i, "Empty string.");
    return string;
}

char **parseString(char *line) {
    int i = 0;
    char **tokens = malloc(sizeof(char *));
    char *delim = " \t\n", *token = strtok(line, delim);

    errorMessage(!tokens, "Malloc mistake.\n");

    while (token != NULL) {
        tokens[i++] = token;
        tokens = realloc(tokens, (i + 1) * sizeof(char *));
        errorMessage(!tokens, "Realloc mistake.\n");

        token = strtok(NULL, delim);
    }

    tokens[i] = NULL;
    return tokens;
}

int sh_launch(char **args) {
    if (strcmp(args[0], "exit") == 0) return 0;
    if (strcmp(args[0], "cd") == 0) return sh_cd(args);
    pid_t pid = fork();

    switch (pid) {
        case -1:
            perror("Fork failed");
            exit(1);
        case 0:
            execvp(args[0], args);
            break;
        default:
            wait(NULL);
    }

    return 1;
}

void onCtrlC() {
    pid_t pid;
    fflush(stdout);
    scanf("%d", &pid); 
    kill(pid, SIGKILL); 
}

void errorMessage(int condition, const char *message) {
    if (condition) {
        printf("%s", message);
        exit(EXIT_FAILURE);
    }
}

int sh_cd(char **args) {
    if (args[1] == NULL) {
        printf("Oжидается аргумент для \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            printf("cd");
        }
    }
    return 1;
}