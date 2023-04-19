#include "archiver.h"

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

    if (i) return string;

    free(string);
    return NULL;
}

void readFile(char *file_name, FILE *archive, FILE *container) {
    FILE *fp = fopen(file_name, "r");
    errorMessage(!fp, "File can't be opened.\n");

    // Writing file name in Header
    fprintf(archive, "%s\n", file_name);

    // Writing file size in Header
    fseek(fp, 0, SEEK_END);
    fprintf(archive, "%ld\n", ftell(fp));
    fseek(fp, 0, SEEK_SET);

    // Writing file content in Container
    readFileContent(fp, container);
    fputc('\n', container);

    fclose(fp);
}

void readFileContent(FILE *stream, FILE *archive) {
    char ch;
    while ((ch = fgetc(stream)) != EOF) {
        fputc(ch, archive);
    }
}

void archiveFiles(FILE *archive, long *cur_header_p, long *cur_container_p) {
    // Reading file name from Archive
    char s[256];
    fseek(archive, *cur_header_p, SEEK_SET);
    fscanf(archive, "%100s\n", s);

    FILE *fp = fopen(s, "w");
    errorMessage(!fp, "File can't be opened.\n");

    // Reading file size from Archive
    long file_size;
    fscanf(archive, "%ld\n", &file_size);

    // Saving current header pointer for next iteration
    *cur_header_p = ftell(archive);

    // Reading file content from Archive
    fseek(archive, *cur_container_p, SEEK_SET);
    for (int i = 0; i < file_size; i++) {
        char ch = fgetc(archive);
        fputc(ch, fp);
    }
    fgetc(archive);

    // Saving current containter pointer for next iteration
    *cur_container_p = ftell(archive);
    fclose(fp);
    printf("File: %s - unzipped successfully.\n", s);
}

void errorMessage(int condition, const char *message) {
    if (condition) {
        printf("%s", message);
        exit(1);
    }
}