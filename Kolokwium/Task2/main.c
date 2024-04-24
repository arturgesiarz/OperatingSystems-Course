#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signal, siginfo_t *info, void*) {
    printf("Otrzymano sygnal %d %c\n", signal, info->si_value.sival_int);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    //..........

    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        pause();
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        int signalNum = atoi(argv[2]);
        int value = atoi(argv[1]);

        union sigval val;
        val.sival_int = value;
        sigqueue(child, signalNum, val);
    }

    return 0;
}
