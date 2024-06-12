#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "time.h"

#include "prot_spec.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage %s <ip> <port>\n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handler);

    // k=Konwersuje addres
    uint32_t ip_address = inet_addr(argv[1]);

    // Pozyskiwanie portu przez konwersje na longa za pomoca strtol
    uint16_t port = (uint16_t) strtol(argv[2], NULL, 10);

    // Struktura adresu serwera
    struct sockaddr_in addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Utworzenie soketu
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    // ...
    int t = 1;

    // ...
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

    // Sprawdzenie czy dany port nie jest zajety
    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        perror("bind");

    // ...
    if (listen(socket_fd, MAX_CLIENTS) < 0)
        perror("listen");

    int clients_fd_array[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients_fd_array[i] = -1;

    bool client_id_set[MAX_CLIENTS] = {0};
    char client_id_array[MAX_CLIENTS][MAX_CLIENT_ID_LEN] = {0};

    // ...
    clock_t clients_alive_timeout[MAX_CLIENTS];

    clock_t ping_time = clock();

    while (!should_close) {
        int client_fd;

        // Czekam na polaczenie
        if((client_fd = accept(socket_fd, NULL, 0)) > 0) {
            int i = 0;
            while (i < MAX_CLIENTS){
                if (clients_fd_array[i] == -1) {
                    clients_fd_array[i] = client_fd;
                    clients_alive_timeout[i] = clock();

                    printf("Client accepted at index: %d\n", i);
                    break;
                }
                i++;
            }

            if (i == MAX_CLIENTS)
                printf("Client limit reached\n");


        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients_fd_array[i] == -1)
                continue;

            request_message_t message;

            // Sprawdzam czy od klienta przyszla jakas wiadomosc, nie blokujac watku
            if(recv(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT) > 0) {
                switch(message.request_type) {
                    case TOALL:
                        printf("TO_ALL: %s FROM: %s \n", message.payload.to_all, message.sender_client_id);
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (clients_fd_array[j] != -1 && i != j) {
                                // Wysylam do wszystkich klientow wiadomosc
                                send(clients_fd_array[j], &message, sizeof(message), MSG_DONTWAIT);
                            }
                        }
                        break;

                    case TOONE:
                        printf("TO_ONE: %s %s FROM: %s \n", message.payload.to_one.target_client,
                               message.payload.to_one.message, message.sender_client_id);
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            // Szukam tego jednego klienta do ktorego chce wyslac
                            if (clients_fd_array[i] != -1 && strncmp(client_id_array[i],
                                 message.payload.to_one.target_client, MAX_CLIENT_ID_LEN) == 0) {
                                // Wysylanie do znalezionego klienta
                                send(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT);
                            }

                        }
                        break;

                    case LIST:
                        printf("LIST FROM: %s\n", message.sender_client_id);
                        int length = 0;
                        for (int j = 0; j < MAX_CLIENTS; j++)
                            if (clients_fd_array[j] != -1){
                                length++;
                                // Kopiowanie wszystkich klientow aby potem ich mogl wyswietlic klient
                                strncpy(message.payload.list.identifiers_list[j], client_id_array[j], MAX_CLIENT_ID_LEN);
                            }
                        message.payload.list.list_length = length;

                        send(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT);
                        break;

                    case ALIVE:
                        // Jesli klient wyslal ALIVE znaczy ze "zyje"
                        printf("ALIVE FROM: %s\n", message.sender_client_id);
                        clients_alive_timeout[i] = clock();
                        if (!client_id_set[i])
                            strncpy(client_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN);
                        break;

                    case STOP:
                        printf("STOP FROM: %s\n", message.sender_client_id);
                        clients_fd_array[i] = -1;
                        client_id_set[i] = false;
                        break;
                }

                // Wyczyszczenie bufora
                fflush(stdout);
            }
        }

        // Wysylanie ALIVE do wszystkich klientow
        if ((clock() - ping_time) / CLOCKS_PER_SEC > 1) {
            request_message_t alive_message = {
                    .request_type = ALIVE
            };
            for (int i = 0; i < MAX_CLIENTS; i++)
                if (clients_fd_array[i] != -1)
                    send(clients_fd_array[i], &alive_message, sizeof(alive_message), MSG_DONTWAIT);
            ping_time = clock();
        }

        // Klient nie odpowiada na ALIVE, a wiec go usuwamy
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients_fd_array[i] != -1 && (clock() - clients_alive_timeout[i]) / CLOCKS_PER_SEC > 5) {
                printf("Client %s timed out\n", client_id_array[i]);
                close(clients_fd_array[i]);
                clients_fd_array[i] = -1;
                client_id_set[i] = false;
            }

    }

    // Zamykamy wszystkich klientow
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients_fd_array[i] != -1)
            close(clients_fd_array[i]);

    return 0;
}