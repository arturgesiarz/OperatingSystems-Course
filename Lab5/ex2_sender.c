#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

    // Pobranie identyfikatora PID procesu catcher oraz trybu pracy
    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    printf("Sender PID: %d\n", getpid());
    printf("Sending SIGUSR1 to catcher\n");

    //  Wysłanie sygnału SIGUSR1 do procesu o podanym identyfikatorze PID
    union sigval value;
    value.sival_int = mode;

    // Wysyłanie sygnałów z danymi do innego procesu
    sigqueue(catcher_pid, SIGUSR1, value);

    // Utworzenie maski sygnałów
    sigset_t wait_mask;
    // Czyszcze maskę sygnałów
    sigemptyset(&wait_mask);
    // Dodaje sygnał SIGUSR1
    sigaddset(&wait_mask, SIGUSR1);
    // Zawieszam wykonywanie procesu, oczekując na sygnał zdefiniowany w masce wait_mask
    sigsuspend(&wait_mask);

    return EXIT_SUCCESS;
}
