#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define BLOCK_SIZE 1024

void copyFileByteByByte(const char *source, const char *destination) {
    int in, out;
    char byte;
    off_t fileSize;

    if ((in = open(source, O_RDONLY)) == -1) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    if ((out = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        perror("Error opening destination file");
        exit(EXIT_FAILURE);
    }

    fileSize = lseek(in, 0, SEEK_END);

    if (fileSize == -1) {
        perror("Error getting file size");
        exit(EXIT_FAILURE);
    }

    for (off_t pos = fileSize - 1; pos >= 0; pos--) {
        if (lseek(in, pos, SEEK_SET) == -1) {
            perror("Error seeking file");
            exit(EXIT_FAILURE);
        }
        if (read(in, &byte, 1) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        if (write(out, &byte, 1) != 1) {
            perror("Error writing file");
            exit(EXIT_FAILURE);
        }
    }

    if (close(in) == -1) {
        perror("Error closing source file");
        exit(EXIT_FAILURE);
    }
    if (close(out) == -1) {
        perror("Error closing destination file");
        exit(EXIT_FAILURE);
    }
}

void copyFileBlockByBlock(const char *source, const char *destination) {
    int in, out;
    char buffer[BLOCK_SIZE];
    ssize_t bytesRead;

    if ((in = open(source, O_RDONLY)) == -1) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    if ((out = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        perror("Error opening destination file");
        exit(EXIT_FAILURE);
    }

    off_t fileSize = lseek(in, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error getting file size");
        exit(EXIT_FAILURE);
    }
    off_t pos = fileSize;

    while (pos > 0) {
        off_t blockSize = (pos >= BLOCK_SIZE) ? BLOCK_SIZE : pos;
        pos -= blockSize;

        if (lseek(in, pos, SEEK_SET) == -1) {
            perror("Error seeking file");
            exit(EXIT_FAILURE);
        }

        if ((bytesRead = read(in, buffer, blockSize)) == -1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }

        for (ssize_t i = bytesRead - 1; i >= 0; i--) {
            if (write(out, &buffer[i], 1) != 1) {
                perror("Error writing file");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (close(in) == -1) {
        perror("Error closing source file");
        exit(EXIT_FAILURE);
    }
    if (close(out) == -1) {
        perror("Error closing destination file");
        exit(EXIT_FAILURE);
    }
}

double measureTime(void (*func)(const char *, const char *), const char *source, const char *destination) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    func(source, destination);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    return cpu_time_used;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // wczytanie standrdowe plikow
//    const char *source_file = "texts/source.txt";
//    const char *destination_file_byte_by_byte = "texts/destination_byte_by_byte.txt";
//    const char *destination_file_block_by_block = "texts/destination_block_by_block.txt";

    const char *source_file = argv[1];
    const char *destination_file = argv[2];

    copyFileBlockByBlock(source_file, destination_file);

    // pomiar czasu
//    FILE *output_file = fopen("texts/pomiar_zad_2.txt", "w");
//    if (output_file == NULL) {
//        perror("Error opening output file");
//        exit(EXIT_FAILURE);
//    }
//    double time_byte_by_byte = measureTime(copyFileByteByByte, source_file, destination_file_byte_by_byte);
//    double time_block_by_block = measureTime(copyFileBlockByBlock, source_file, destination_file_block_by_block);
//    fprintf(output_file, "Time for copying byte by byte: %f seconds\n", time_byte_by_byte);
//    fprintf(output_file, "Time for copying block by block: %f seconds\n", time_block_by_block);
//    fclose(output_file);

    return 0;
}