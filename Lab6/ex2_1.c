#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    double start, end;

    printf("Enter the start of the interval: ");
    scanf("%lf", &start);

    printf("Enter the end of the interval: ");
    scanf("%lf",&end);

    if (mkfifo("pipe", 0666) == -1) {
        perror("Failed to create named pipe");
        exit(1);
    }

    int fd = open("pipe", O_WRONLY);
    if (fd == -1) {
        perror("Failed to open named pipe for writing");
        exit(1);
    }

    if (write(fd, &start, sizeof(double)) == -1 || write(fd, &end, sizeof(double)) == -1) {
        perror("Write to pipe failed");
        exit(1);
    }

    close(fd);

    fd = open("pipe", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open named pipe for reading");
        exit(1);
    }

    double result;
    if (read(fd, &result, sizeof(double)) == -1) {
        perror("Read from pipe failed");
        exit(1);
    }

    close(fd);
    unlink("pipe");
    printf("Result: %f\n", result);

    return 0;
}
