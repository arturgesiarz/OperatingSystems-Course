#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <error.h>

int main() {
  pid_t pid;
  time_t t;
  int status;

  if ((pid = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
  } else if (pid == 0) {
      sleep(5);
      exit(5);

  } else {
//  sprawdź status procesu potomka, nie zawieszając przy tym procesu macierzystego
//  1) W przypadku błędu zwróć go.
//  2) W przypadku, gdy proces się  jeszcze nie zakończył:
//     - pobierz aktualną datę.
//     - wypisz "Proces potomny nadal pracuje %data%"
//     - uśpij proces macierzysty na jedną sekundę
        do {
            if (waitpid(pid, &status, WNOHANG) < 0) {
                exit(EXIT_FAILURE);
            }
            if (!WIFEXITED(status)) {
                t = time(NULL);
                printf("Proces potomny nadal pracuje %s", ctime(&t));
                sleep(1);
            }
        }
        while (!WIFEXITED(status));

//  koniec
//  ciąg dalszy do while , przypadek, gdy proces potomny się zakończył

        if (WIFEXITED(status)) {
            printf("Proces potomny zakonczył się statusem %d\n", WEXITSTATUS(status));
        } else {
            puts("Proces potomny nie zakonczyl sie ");
        }

  }

  return 0;
}
