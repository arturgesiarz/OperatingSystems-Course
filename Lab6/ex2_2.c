#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

double function(double x) {
    return 4 / (x * x + 1);
}

double calculate_integral(double start, double end, double width) {
    double sum = 0;
    double x;
    for (x = start; x < end; x += width) {
        sum += function(x) * width;
    }
    return sum;
}

int main() {
    double start, end;

    int fd = open("pipe", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open named pipe for reading");
        exit(1);
    }

    if (read(fd, &start, sizeof(double)) == -1 || read(fd, &end, sizeof(double)) == -1) {
        perror("Read from pipe failed");
        exit(1);
    }

    close(fd);

    double width = 0.001;
    double result = calculate_integral(start, end, width);

    fd = open("pipe", O_WRONLY);
    if (fd == -1) {
        perror("Failed to open named pipe for writing");
        exit(1);
    }

    if (write(fd, &result, sizeof(double)) == -1) {
        perror("Write to pipe failed");
        exit(1);
    }

    close(fd);
    return 0;
}
