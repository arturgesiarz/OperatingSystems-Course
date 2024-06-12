#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "prot_spec.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handler);

    // Konwersja adresu
    uint32_t ip_address = inet_addr(argv[1]);

    // Konwersja portu
    uint16_t port = (uint16_t) strtol(argv[2], NULL, 10);

    struct sockaddr_in server_addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Tworzenie soketu datagramowego
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Bindowanie soketu
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        perror("bind");

    // Tworzenie tablicy z nazwami klientow
    char client_id_array[MAX_CLIENTS][MAX_CLIENT_ID_LEN] = {0};

    // Tworzenie adresu dla kazdego klienta
    struct sockaddr_in client_addr_array[MAX_CLIENTS];

    bool client_id_set[MAX_CLIENTS] = {0};
    clock_t clients_alive_timeout[MAX_CLIENTS];

    clock_t ping_time = clock();

    while (!should_close) {
        // Nie sprawdzamy polaczenia - poniewaz charaktersytyka soketu datagramu jest sama w sobie
        // bezpolaczeniowa

        // Tworzenie sockaddr_in dla klienta
        struct sockaddr_in client_addr;

        socklen_t addr_len = sizeof(client_addr);

        // Tworzymy wiadomosc
        request_message_t message;

        if (recvfrom(socket_fd, &message, sizeof(message), MSG_DONTWAIT,
                     (struct sockaddr*)&client_addr, &addr_len) > 0) {

            // Szukam klienta ktory wyslal wiadomosc
            int client_index = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_id_set[i] && strncmp(client_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN) == 0) {
                    client_index = i;
                    break;
                }
            }

            // Jesli nie znalazlem klienta, tworze go na najblizszym wolnym slocie
            if (client_index == -1) {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!client_id_set[i]) {
                        client_index = i;
                        strncpy(client_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN);
                        client_addr_array[i] = client_addr;
                        client_id_set[i] = true;
                        break;
                    }
                }
            }

            // Jesli udalo sie znalezc klienta lub utworzyc, to przystepuje do dalszej czesci
            if (client_index != -1) {
                clients_alive_timeout[client_index] = clock();

                switch(message.request_type) {
                    case TOALL:
                        printf("TO_ALL: %s FROM: %s \n", message.payload.to_all, message.sender_client_id);
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_id_set[j] && j != client_index) {
                                sendto(socket_fd, &message, sizeof(message), 0,
                                       (struct sockaddr*)&client_addr_array[j], addr_len);
                            }
                        }
                        break;

                    case TOONE:
                        printf("TO_ONE: %s %s FROM: %s \n", message.payload.to_one.target_client,
                               message.payload.to_one.message, message.sender_client_id);
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            if (client_id_set[i] && strncmp(client_id_array[i], message.payload.to_one.target_client, MAX_CLIENT_ID_LEN) == 0) {
                                sendto(socket_fd, &message, sizeof(message), 0,
                                       (struct sockaddr*)&client_addr_array[i], addr_len);
                                break;
                            }
                        }
                        break;

                    case LIST:
                        printf("LIST FROM: %s\n", message.sender_client_id);
                        int length = 0;
                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_id_set[j]) {
                                strncpy(message.payload.list.identifiers_list[length], client_id_array[j],
                                        MAX_CLIENT_ID_LEN);
                                length++;
                            }
                        }
                        message.payload.list.list_length = length;
                        sendto(socket_fd, &message, sizeof(message), 0,
                               (struct sockaddr*)&client_addr, addr_len);
                        break;

                    case ALIVE:
                        printf("ALIVE FROM: %s\n", message.sender_client_id);
                        if (!client_id_set[client_index])
                            strncpy(client_id_array[client_index], message.sender_client_id, MAX_CLIENT_ID_LEN);
                        break;

                    case STOP:
                        printf("STOP FROM: %s\n", message.sender_client_id);
                        client_id_set[client_index] = false;
                        break;
                }

                fflush(stdout);
            }
        }

        if ((clock() - ping_time) / CLOCKS_PER_SEC > 1) {
            request_message_t alive_message = {
                    .request_type = ALIVE
            };
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_id_set[i]) {
                    sendto(socket_fd, &alive_message, sizeof(alive_message), 0, (struct sockaddr*)&client_addr_array[i], addr_len);
                }
            }
            ping_time = clock();
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_id_set[i] && (clock() - clients_alive_timeout[i]) / CLOCKS_PER_SEC > 5) {
                printf("Client %s timed out\n", client_id_array[i]);
                client_id_set[i] = false;
            }
        }
    }

    close(socket_fd);

    return 0;
}
