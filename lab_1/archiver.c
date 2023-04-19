#include "archiver.h"

int main(int argc, char **argv) {
    errorMessage(argc != 2, "Invalid parameter.\n");

    if (strcmp(argv[1], "zip") == 0 || strcmp(argv[1], "0") == 0) {
        printf("Enter full directory path: ");
        char *dir_path = setString();
        errorMessage(!dir_path, "Invalid full directory path.\n");

        DIR *dir;
        errorMessage(chdir(dir_path) || (dir = opendir("./")) == NULL,
                     "Directory can't be opened.\n");

        // Getting folder_name to delete on exit
        char *tname = strrchr(dir_path, '/');
        char *folder_name;
        if (!tname)
            folder_name = strdup(dir_path);
        else
            folder_name = strdup(tname + 1);

        // Getting archive name as initial folder name + .archive
        char *archive_name = malloc(sizeof(char) * (strlen(folder_name) + 9));
        strcpy(archive_name, folder_name);
        strcat(archive_name, ".archive");

        // Creating temporary files and archive file
        chdir("..");
        FILE *archive = fopen(archive_name, "w");
        FILE *container = tmpfile();
        FILE *header = tmpfile();
        chdir(dir_path);

        // Archiving and than removing files from folder
        printf("\n");
        struct dirent *pd;
        while ((pd = readdir(dir)) != NULL) {
            if (strcmp(pd->d_name, ".") == 0 || strcmp(pd->d_name, "..") == 0) continue;
            readFile(pd->d_name, header, container);
            remove(pd->d_name);
            printf("File: %s - archived successfully.\n", pd->d_name);
        }

        // Writing header size in the beginnig of archive
        fprintf(archive, "%ld\n", ftell(header) - 1);
        fseek(container, 0, SEEK_SET);
        fseek(header, 0, SEEK_SET);

        // Filling archive file with header content
        readFileContent(header, archive);
        // Filling archive file with files content
        readFileContent(container, archive);

        fclose(container);
        fclose(archive);
        fclose(header);
        closedir(dir);

        // removing folder and temporary file
        chdir("..");
        rmdir(folder_name);

        free(dir_path);
        free(folder_name);
        free(archive_name);

        printf("\nAll files was archived successfully.\n");
    }

    if (strcmp(argv[1], "unzip") == 0 || strcmp(argv[1], "1") == 0) {
        printf("Enter full archive path: ");
        char *archive_path = setString();
        errorMessage(
            !archive_path || !strcmp(&archive_path[strlen(archive_path) - 8], ".archive") == 0,
            "Invalid full archive path.\n");

        // Getting archive_name to delete on exit and to get folder name - .archive
        char *tname = strrchr(archive_path, '/');
        char *archive_name;
        if (!tname)
            archive_name = strdup(archive_path);
        else
            archive_name = strdup(tname + 1);

        // Getting folder name without .archive
        char *folder_name = strdup(archive_name);
        strtok(folder_name, ".");

        FILE *archive = fopen(archive_path, "r");
        errorMessage(!archive, "Archive can't be opened.\n");

        // Getting the end of the archive file //to comparing
        fseek(archive, 0, SEEK_END);
        long archive_size = ftell(archive);
        fseek(archive, 0, SEEK_SET);

        // Getting size of a header and initializing current_pointers to work with archive file
        long header_size, cur_header_p, cur_container_p;
        fscanf(archive, "%ld\n", &header_size);
        cur_header_p = ftell(archive);
        cur_container_p = cur_header_p + header_size + 1;

        mkdir(folder_name, 0777);
        chdir(folder_name);

        printf("\n");
        while (cur_container_p != archive_size) {
            archiveFiles(archive, &cur_header_p, &cur_container_p);
        }

        chdir("..");
        remove(archive_name);

        fclose(archive);
        free(archive_path);
        free(archive_name);
        free(folder_name);

        printf("\nAll files was unzipped successfully.\n");
    }

    return 0;
}
