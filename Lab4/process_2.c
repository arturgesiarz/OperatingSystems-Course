#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    char *directory_path = argv[1];
    int local = 0;

    printf("Nazwa programu: %s\n", argv[0]);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        printf("child process\n");
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        local++;
        global++;
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "ls", directory_path, NULL);
        perror("execl");
        return 1;
    } else {
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), pid);
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("child exit code: %d\n", WEXITSTATUS(status));
        } else {
            printf("descendant process failed!\n");
        }
        printf("parent's local = %d, parent's global = %d\n", local, global);
        return 0;
    }
}