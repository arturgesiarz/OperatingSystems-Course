#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

double function(double x) {
    return 4 / (x * x + 1);
}

double integrate_function() {
    return M_PI;
}

double calculate_integral(double start, double end, double width) {
    double sum = 0;
    double x;
    for (x = start; x < end; x += width) {
        sum += function(x) * width;
    }
    return sum;
}

int main(int argc, char *argv[]) {

    // atof - zamienia stringa na double
    double width = atof(argv[1]);

    // atoi - zamienia stringa na inta
    int num_processes = atoi(argv[2]);

    // Obliczam dlugosc przedzialu
    double interval = 1.0 / num_processes;

    // Tworze tablicę potoków, które będą używane
    // do komunikacji między procesami potomnymi a procesem macierzystym
    int fd[num_processes][2];
    double results[num_processes];

    // Iteracja po wszystkich procesach
    for (int i = 0; i < num_processes; i++) {

        // tworzenie potoku nienazwowego
        if (pipe(fd[i]) == -1) {
            perror("Pipe creation failed");
            return 1;
        }

        pid_t pid = fork();

        // proces macierzysty
        if (pid == -1) {
            perror("Fork failed");
            return 1;

        }

        // proces potomny
        else if (pid == 0) {
            close(fd[i][0]);

            double start = i * interval;
            double end = start + interval;
            double result = calculate_integral(start, end, width);

            // zapisywanie danych
            write(fd[i][1], &result, sizeof(double));

            // zamykam potok
            close(fd[i][1]);
            return 0;
        }
    }

    for (int i = 0; i < num_processes; i++) {
        close(fd[i][1]);
        read(fd[i][0], &results[i], sizeof(double));
        close(fd[i][0]);
    }

    double total_result = 0;
    for (int i = 0; i < num_processes; i++) {
        total_result += results[i];
    }

    printf("Total result: %f\n", total_result);
    printf("Real value is: %f\n", integrate_function());
    return 0;
}
