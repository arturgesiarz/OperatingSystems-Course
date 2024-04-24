#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_CLIENTS 10
#define SERVER_QUEUE_KEY 1234

struct message {
    long mtype;
    char mtext[256];
};

int main() {
    int server_queue;
    int client_queues[MAX_CLIENTS];
    int num_clients = 0;

    // Tworzenie kolejki serwera
    if ((server_queue = msgget(SERVER_QUEUE_KEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Serwer uruchomiony. Oczekiwanie na klientów...\n");

    while (1) {
        struct message msg;
        // Odbieranie komunikatów od klientów
        if (msgrcv(server_queue, &msg, sizeof(struct message), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        // Obsługa komunikatu INIT

        if (strcmp(msg.mtext, "INIT") == 0) {
            if (num_clients < MAX_CLIENTS) {
                // Tworzenie nowej kolejki klienta
                key_t client_key = ftok("/tmp", num_clients);
                int client_queue = msgget(client_key, IPC_CREAT | 0666);
                if (client_queue == -1) {
                    perror("msgget");
                    exit(1);
                }
                client_queues[num_clients++] = client_queue;

                // Wysyłanie identyfikatora klientowi
                snprintf(msg.mtext, sizeof(msg.mtext), "%d", num_clients);
                msg.mtype = 1;
                if (msgsnd(client_queue, &msg, sizeof(struct message), 0) == -1) {
                    perror("msgsnd");
                    exit(1);
                }
            } else {
                printf("Osiągnięto maksymalną liczbę klientów.\n");
            }
        } else {
            // Przekazywanie komunikatu od klienta do wszystkich pozostałych klientów
            for (int i = 0; i < num_clients; i++) {
                if (client_queues[i] != msg.mtype) {
                    if (msgsnd(client_queues[i], &msg, sizeof(struct message), 0) == -1) {
                        perror("msgsnd");
                        exit(1);
                    }
                }
            }
        }
    }

    return 0;
}
