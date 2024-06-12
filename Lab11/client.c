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

#include "prot_spec.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port> <self identifier> \n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handler);

    char* client_identifier = argv[1];

    // konwersja adresu IP
    uint32_t ip_address = inet_addr(argv[2]);

    // konwersja portu
    uint16_t port = (uint16_t)strtol(argv[3], NULL, 10);

    // Struktura adresu serwera
    struct sockaddr_in addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Utworzenie soketu
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Nawiązanie połączenia z serwerem
    if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        perror("connect");

    // Utworzenie wiadomości ALIVE
    request_message_t alive_message = {
            .request_type = ALIVE
    };

    // Skopiowanie identyfikayora do alive_message
    strncpy(alive_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);

    // Wyslanie do serwera alive_message
    send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);

    pid_t listener_pid = fork();

    if (listener_pid < 0)
        perror("fork listener");

    // Proces potomny - jest do wyswietlania wynikow
    else if (listener_pid == 0) {
        while (!should_close) {
            request_message_t message;

            // Blokowanie watku poki nie otrzymamy wiadomosci
            recv(socket_fd, &message, sizeof(message), MSG_WAITALL);

            // Wyswietlamy wyniki jakie nam przetworzyl serwer
            switch(message.request_type) {
                case LIST:
                    for (int i = 0; i < message.payload.list.list_length; i++) {
                        printf("%s\n", message.payload.list.identifiers_list[i]);
                    }
                    break;
                case TOALL:
                    printf("TOALL FROM: %s: %s\n", message.sender_client_id, message.payload.to_all);
                    break;
                case TOONE:
                    printf("TOONE FROM: %s: %s\n", message.sender_client_id, message.payload.to_one.message);
                    break;
                case ALIVE:
                    send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);
                    break;
                default:
                    printf("Invalid response type! \n");
                    break;
            }
        }
    }
    // Proces macierzysty - jest do laczenia sie z serwerem
    else {
        char* request_type_input_buffer = NULL;

        // Wysylanie do serwera konkretnych zapytan, ktore chcemy osiaganac
        while (!should_close) {
            if(scanf("%ms", &request_type_input_buffer) == 1) {
                request_message_t message;
                strncpy(message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);
                if (strncmp(request_type_input_buffer, "LIST", 4) == 0){
                    message.request_type = LIST;
                    send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
                }
                else if (strncmp(request_type_input_buffer, "2ALL", 4) == 0) {
                    message.request_type = TOALL;
                    scanf("%s", message.payload.to_all);
                    send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
                }
                else if (strncmp(request_type_input_buffer, "2ONE", 4) == 0) {
                    message.request_type = TOONE;
                    scanf("%s", message.payload.to_one.target_client);
                    scanf("%s", message.payload.to_one.message);
                    send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
                } else {
                    printf("Invalid request type! \n");
                }

                free(request_type_input_buffer);
            } else
                perror("scanf input");

        }

        request_message_t stop_message = {
                .request_type = STOP
        };

        strncpy(stop_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);
        send(socket_fd, &stop_message, sizeof(stop_message), MSG_DONTWAIT);
    }

    close(socket_fd);

    return 0;
}