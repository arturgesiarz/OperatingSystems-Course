#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
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

    // Read the interval from standard input
    printf("Enter the start and end of the interval: ");
    scanf("%lf %lf", &start, &end);

    // Create named pipe
    if (mkfifo("pipe", 0666) == -1) {
        perror("Failed to create named pipe");
        exit(1);
    }

    // Open the named pipe for writing
    int fd = open("pipe", O_WRONLY);
    if (fd == -1) {
        perror("Failed to open named pipe for writing");
        exit(1);
    }

    // Calculate the integral for the given interval
    double width = 0.001; // Example width
    double result = calculate_integral(start, end, width);

    // Write the result to the named pipe
    if (write(fd, &result, sizeof(double)) == -1) {
        perror("Write to pipe failed");
        exit(1);
    }

    // Close the named pipe
    close(fd);

    // Remove the named pipe
    unlink("pipe");

    printf("Result: %f\n", result);

    return 0;
}
