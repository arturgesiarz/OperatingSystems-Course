#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define REINDEER_NUMBER 9
#define MAX_GIFTS 4

pthread_mutex_t mutexReindeer;

int reindeer_available = 0;
int gifts_delivered = 0;

// funkcja wysylajaca na wakacje danego renifera
void summer_time(int* reindeerID) {
    int sleep_time = 5 + rand() % 6;
    srand(time(NULL));
    printf("Reindeer no: %d. There is no available.\n", *reindeerID);
    sleep(sleep_time);
}

// funkcja renifera
void* reindeer(void* arg) {
    int* reindeerID = (int*) arg;
    summer_time(reindeerID);
}


// funkcja mikolaja
void* santa(void* arg) {
}

int main() {
    pthread_t reindeer_th[REINDEER_NUMBER];
    pthread_t santa_th;

    // tablica trzymajaca ID wszystkich reniferow
    int idsReindeer[REINDEER_NUMBER];

    // tworzenie mutexow
    pthread_mutex_init(&mutexReindeer, NULL);

    // tworzenie watkow refierow
    for (int i = 0; i < REINDEER_NUMBER; i++) {
        idsReindeer[i] = i;
        if (pthread_create(&reindeer_th[i], NULL, &reindeer, &idsReindeer[i]) != 0) {
            perror("Failed to created thread");
        }
    }

    // tworzenie watku mikokaja
    if (pthread_create(&santa_th, NULL, &santa, NULL) != 0) {
        perror("Failed to created thread");
    }

    // czekanie az wszystkie watki sie zakoncza
    for (int i = 0; i < REINDEER_NUMBER; i++) {
        if (pthread_join(reindeer_th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    // niszczenie mutexow
    pthread_mutex_destroy(&mutexReindeer);

   return 0;
}