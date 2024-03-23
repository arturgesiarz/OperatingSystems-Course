#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    long long total_size = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }
        if (!S_ISDIR(file_stat.st_mode)) {
            printf("File: %s, Size: %lld bytes\n", entry->d_name, (long long)file_stat.st_size);
            total_size += file_stat.st_size;
        }
    }

    closedir(dir);

    printf("Total size of all files: %lld bytes\n", total_size);
    return 0;
}
