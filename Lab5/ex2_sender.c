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

    for (int i = 0; i < 10; ++i) {
        printf("Sending SIGUSR1 to catcher\n");
        //  Wysłanie sygnału SIGUSR1 do procesu o podanym identyfikatorze PID
        union sigval value;
        value.sival_int = mode;
        sigqueue(catcher_pid, SIGUSR1, value);

        // Utworzenie maski sygnałów
        sigset_t wait_mask;
        sigemptyset(&wait_mask);
        sigaddset(&wait_mask, SIGUSR1);
        sigsuspend(&wait_mask);
    }

    return EXIT_SUCCESS;
}
