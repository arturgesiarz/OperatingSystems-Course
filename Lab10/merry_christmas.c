#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define REINDEER_NUMBER 9
#define MAX_GIFTS 4

// mutex oraz cond-mutex dla mikolaja w celu odliczania ile prezentow on dostarczyl
pthread_mutex_t mutexGift = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condGift = PTHREAD_COND_INITIALIZER;

// licznik ile aktualnie reniferow jest dostepnych
pthread_cond_t condReindeer = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexReindeer = PTHREAD_MUTEX_INITIALIZER;

int reindeer_available = REINDEER_NUMBER;
int gifts_delivered = 0;

// funkcja wysylajaca na wakacje danego renifera
void summer_time(int* reindeerID) {
    srand(time(NULL));
    int sleep_time = 5 + rand() % 6;
    printf("Reindeer no: %d. There is no available for %d seconds.\n", *reindeerID, sleep_time);
    sleep(sleep_time);
}

//funkcja dostarczajaca zabawki
void give_toys() {
    srand(time(NULL));
    int sleep_time = 2 + rand() % 3;
    printf("Santa's giving toys!");
    sleep(sleep_time);
}

// funkcja renifera
void* reindeer(void* arg) {
    // pobieranie ID danego renifera
    int* reindeerID = (int*) arg;

    while (gifts_delivered < MAX_GIFTS) {

        // przed poslaniem na wakacje odejmujemy liczbe dostepnych reniferow
        pthread_mutex_lock(&mutexReindeer);
        reindeer_available -= 1;
        pthread_mutex_unlock(&mutexReindeer);

        // posylanie go na wakacje
        summer_time(reindeerID);

        // teraz nalezy zwiekszyc liczbe dostepnych reniferow bo renifer jest po wakacjach
        pthread_mutex_lock(&mutexReindeer);
        reindeer_available += 1;
        pthread_mutex_unlock(&mutexReindeer);
        pthread_cond_signal(&condReindeer);

        // usypiamy watek opoczynku reniferow
        pthread_cond_wait(&condGift, &mutexGift);
    }
}


// funkcja mikolaja
void* santa(void* arg) {
    for (int i = 0; i < MAX_GIFTS; i++) {

        // pozyskanie odpowiedniej ilosci reniferow
        pthread_mutex_lock(&mutexReindeer);
        while (reindeer_available < REINDEER_NUMBER) {
            printf("Santa's waiting for reindeer. Number of reindeer is %d. There's %d left\n",
                   reindeer_available, REINDEER_NUMBER - reindeer_available);
            pthread_cond_wait(&condReindeer, &mutexReindeer);
        }
        printf("Santa's waking up!");
        pthread_mutex_unlock(&mutexReindeer);

        // dostarczenie zabawek
        pthread_mutex_lock(&mutexGift);
        give_toys();
        gifts_delivered += 1;
        printf("Santa's going to sleep!");
        pthread_mutex_unlock(&mutexGift);
        pthread_cond_broadcast(&condGift);
    }
}

int main() {
    pthread_t reindeer_th[REINDEER_NUMBER];
    pthread_t santa_th;

    // tablica trzymajaca ID wszystkich reniferow
    int idsReindeer[REINDEER_NUMBER];

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
    pthread_mutex_destroy(&mutexGift);
    pthread_cond_destroy(&condGift);
    pthread_cond_destroy(&condReindeer);

   return 0;
}