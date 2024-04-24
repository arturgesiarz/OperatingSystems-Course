#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_QUEUE_KEY 1234

struct message {
    long mtype;
    char mtext[256];
};

int main() {
    int server_queue;

    // Uzyskiwanie dostępu do kolejki serwera
    if ((server_queue = msgget(SERVER_QUEUE_KEY, 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    // Wysyłanie komunikatu INIT do serwera
    struct message init_msg;
    init_msg.mtype = SERVER_QUEUE_KEY;
    strcpy(init_msg.mtext, "INIT");
    if (msgsnd(server_queue, &init_msg, sizeof(struct message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Oczekiwanie na identyfikator...\n");

    // Odbieranie identyfikatora od serwera
    struct message client_msg;
    if (msgrcv(server_queue, &client_msg, sizeof(struct message), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    int client_queue_id = atoi(client_msg.mtext);
    printf("Otrzymano identyfikator klienta: %d\n", client_queue_id);

    // Tworzenie nowego procesu do odbierania komunikatów od serwera
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // Proces dziecka - odbieranie komunikatów
        struct message msg;
        while (1) {
            if (msgrcv(client_queue_id, &msg, sizeof(msg.mtext), 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }
            printf("Otrzymano wiadomość od serwera: %s\n", msg.mtext);
        }
    } else { // Proces rodzica - wysyłanie komunikatów
        struct message msg;
        while (1) {
            printf("Wpisz wiadomość: ");
            fgets(msg.mtext, sizeof(msg.mtext), stdin);
            msg.mtype = SERVER_QUEUE_KEY;
            if (msgsnd(server_queue, &msg, sizeof(msg.mtext), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
        }
    }

    return 0;
}
