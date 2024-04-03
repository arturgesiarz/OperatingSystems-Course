#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// Definicja zmiennej globalnej,
// która będzie przechowywać liczbę odebranych sygnałów.
// Zmienna ta jest zadeklarowana jako volatile sig_atomic_t,
// co oznacza, że jest to zmienna atomowa, która może być bezpiecznie modyfikowana wewnątrz sygnałów.
volatile sig_atomic_t received_signals = 0;


// Funkcja ta zostanie wywołana, gdy proces otrzyma sygnał SIGUSR1.
void sigusr1_handler(int signum, siginfo_t *info, void *context) {
    printf("Catcher received SIGUSR1 from sender PID: %d\n", info->si_pid);
    received_signals++;

    // Wysłanie sygnału SIGUSR1 z powrotem do procesu, który go wysłał
    sigqueue(info->si_pid, SIGUSR1, (union sigval){});

    switch (info->si_value.sival_int) {
        case 1:
            for (int i = 1; i <= 100; ++i) {
                printf("%d\n", i);
                // Wyczyszczenia bufora wyjściowego dla strumienia
                fflush(stdout);
            }
            break;
        case 2:
            printf("Received %d requests to change mode\n", received_signals);
            break;
        case 3:
            printf("Exiting catcher\n");
            exit(EXIT_SUCCESS);
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigusr1_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    printf("Catcher PID: %d\n", getpid());

    while(1) {
        pause();
    }

    return EXIT_SUCCESS;
}
