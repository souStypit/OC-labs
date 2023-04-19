#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char *setString();
void readFile(char *file_name, FILE *archive, FILE *container);
void readFileContent(FILE *stream, FILE *archive);
void archiveFiles(FILE *archive, long *cur_header_p, long *cur_container_p);
void errorMessage(int condition, const char *message);