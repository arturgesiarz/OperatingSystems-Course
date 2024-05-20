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

int reindeer_available = 0;
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
    printf("Santa's giving toys!\n");
    sleep(sleep_time);
}

// funkcja renifera
void* reindeer(void* arg) {
    // pobieranie ID danego renifera
    int* reindeerID = (int*) arg;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (gifts_delivered < MAX_GIFTS) {

        // posylanie go na wakacje
        summer_time(reindeerID);

        // teraz nalezy zwiekszyc liczbe dostepnych reniferow bo renifer jest po wakacjach
        pthread_mutex_lock(&mutexReindeer);
        reindeer_available += 1;

        // budze mikolaja wtedy kiedy mam do dyspozycji 9 reniferow
        if (reindeer_available == 9) {
            pthread_cond_signal(&condReindeer);
            reindeer_available = 0;
        }
        pthread_mutex_unlock(&mutexReindeer);

        // usypiamy watek opoczynku reniferow
        pthread_mutex_lock(&mutexGift);
        pthread_cond_wait(&condGift, &mutexGift);
        pthread_mutex_unlock(&mutexGift);
    }
}


// funkcja mikolaja
void* santa(void* arg) {
    for (int i = 0; i < MAX_GIFTS; i++) {

        // czekam na sygnal ze mam juz dostepnych 9 reniferow
        pthread_cond_wait(&condReindeer, &mutexReindeer);
        printf("Santa's waking up!\n");

        // dostarczenie zabawek
        pthread_mutex_lock(&mutexGift);
        give_toys();
        gifts_delivered += 1;
        printf("Santa's going to sleep!\n");
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