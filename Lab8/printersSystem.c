#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include "sharedMemorySpces.h"

volatile bool should_close = false;

// Funkcja uzywana do zamkniecia klienta
void SIGNAL_handler(int signum) {
    should_close = true;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        return -1;
    }

    // Konwertuje liczbe wejsciowa na longa
    long number_of_printers = strtol(argv[1], NULL, 10);

    // Sprawdzam czy liczba drukarek nie jest za duza
    if (number_of_printers > MAX_PRINTERS) {
        printf("Number of printers is too big, maximum number of printers is %d\n", MAX_PRINTERS);
        return -1;
    }

    // Proba utworzenia i otwarcia deskryptora pliku dla pamieci wspoldzielonej
    int memory_fd = shm_open(SHARED_MEMORY_DESCRIPTOR_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR);
    if(memory_fd < 0)
        perror("shm_open");

    // Okreslenia rozmiaru pamieci wspoldzielonej
    if(ftruncate(memory_fd, sizeof(memoryMapT)) < 0)
        perror("ftruncate");

    // Próba zmapowania regionu pamięci współdzielonej do przestrzeni adresowej programu
    memoryMapT* memory_map = mmap(NULL, sizeof(memoryMapT), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);

    if (memory_map == MAP_FAILED)
        perror("mmap");

    // Czyszczenie regionu pamięci współdzielonej
    memset(memory_map, 0, sizeof(memoryMapT));

    // Ustawiam liczbe dostepnych drukarek
    memory_map->numberOfPrinters = number_of_printers;

    // Zarejestruj obsługę sygnału, aby zamknąć klienta na wszystkie sygnały
    for (int sig = 1; sig < SIGRTMAX; sig++) {
        signal(sig, SIGNAL_handler);
    }

    // Generowanie wszystkich drukarek
    for (int i = 0; i < number_of_printers; i++){

        // Inicjalizacja nienazwanych semaforów
        sem_init(&memory_map->printers[i].printerSemaphore, 1, 1);

        pid_t printer_pid = fork();

        if(printer_pid < 0) {
            perror("fork");
            return -1;
        }
        else if(printer_pid == 0) {

            // Proces drukarki - oczekiwanie na żądanie drukowania i drukowanie danych
            while(!should_close) {

                // Jeśli użytkownik zasygnalizował, że drukarka powinna drukować,
                // rozpocznij drukowanie danych z bufora
                if (memory_map->printers[i].printerState == PRINTING) {

                    // Drukowanie danych z bufora z opóźnieniem 1s po każdym znaku
                    for (int j = 0; j < memory_map->printers[i].printerBufferSize; j++) {
                        printf("%c", memory_map->printers[i].printerBuffer[j]);
                        sleep(1);
                    }

                    // Dodanie nowej linii
                    printf("\n");
                    fflush(stdout);

                    // Resetowanie stanu drukarki
                    memory_map->printers[i].printerState = WAITING;

                    // Zwiększenie semafora w celu zasygnalizowania, że drukowanie zostało wykonane
                    sem_post(&memory_map->printers[i].printerSemaphore);
                }
            }
            exit(0);
        }
    }

    // Oczekiwanie na zakończenie pracy wszystkich drukarek
    while(wait(NULL) > 0) {}

    // Zniszczenie semaformow
    for (int i = 0; i < number_of_printers; i++)
        sem_destroy(&memory_map->printers[i].printerSemaphore);


    // Odmapowanie obszaru pamieci
    munmap(memory_map, sizeof(memoryMapT));

    // Zamknięcie deskryptora pliku pamięci współdzielonej
    shm_unlink(SHARED_MEMORY_DESCRIPTOR_NAME);
}