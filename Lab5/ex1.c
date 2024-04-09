#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void sigusr1_handler() {
    printf("Received SIGUSR1\n");
}

int main(int argc, char *argv[]) {

    // Tworzenie i inicjalizacja zbioru sygnałów oraz dodanie do tego zbioru sygnału SIGUSR1
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    if (strcmp(argv[1], "none") == 0) {
        // nothing
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    } else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, sigusr1_handler);
    } else if (strcmp(argv[1], "mask") == 0) {
        // Nowe sygnały zostaną dodane do aktualnej maski sygnałów,
        sigprocmask(SIG_BLOCK, &mask, NULL);
    } else {
        fprintf(stderr, "Invalid argument\n");
        return EXIT_FAILURE;
    }

    printf("Running with argument: %s\n", argv[1]);

    // Wysłanie sygnału SIGUSR1 do samego siebie.
    raise(SIGUSR1);

    // Sprawdzenie, czy sygnał SIGUSR1 znajduje się w zbiorze mask.
    if (sigismember(&mask, SIGUSR1)) {
        printf("SIGUSR1 is pending\n");
    }

    return EXIT_SUCCESS;
}
