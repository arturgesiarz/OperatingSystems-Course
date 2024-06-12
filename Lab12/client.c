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
#include "prot_spec.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <self identifier> <ip> <port>\n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handler);

    char* client_identifier = argv[1];
    uint32_t ip_address = inet_addr(argv[2]);
    uint16_t port = (uint16_t)strtol(argv[3], NULL, 10);

    struct sockaddr_in server_addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Tworze soket datagramowy
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    request_message_t alive_message = {
            .request_type = ALIVE
    };
    strncpy(alive_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);

    // Wyslam wiadomosc alive
    sendto(socket_fd, &alive_message, sizeof(alive_message), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    pid_t listener_pid = fork();

    if (listener_pid < 0)
        perror("fork listener");

    // Proces potomny - jest do wyswietlania wynikow
    else if (listener_pid == 0) {
        while (!should_close) {
            request_message_t message;
            socklen_t addr_len = sizeof(server_addr);

            // Blokowanie watku poki nie otrzymamy wiadomosci
            recvfrom(socket_fd, &message, sizeof(message), 0,
                     (struct sockaddr*)&server_addr, &addr_len);

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
                    sendto(socket_fd, &alive_message, sizeof(alive_message), 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
                    break;
                default:
                    printf("Invalid response type! \n");
                    break;
            }
        }
    }
    // Proces macierzysty - jest do komunikacji z serwerem
    else {
        char* request_type_input_buffer = NULL;

        while (!should_close) {
            if(scanf("%ms", &request_type_input_buffer) == 1) {
                request_message_t message;
                strncpy(message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);

                if (strncmp(request_type_input_buffer, "LIST", 4) == 0){
                    message.request_type = LIST;
                    sendto(socket_fd, &message, sizeof(message), 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
                }
                else if (strncmp(request_type_input_buffer, "2ALL", 4) == 0) {
                    message.request_type = TOALL;
                    scanf("%s", message.payload.to_all);
                    sendto(socket_fd, &message, sizeof(message), 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
                }
                else if (strncmp(request_type_input_buffer, "2ONE", 4) == 0) {
                    message.request_type = TOONE;
                    scanf("%s", message.payload.to_one.target_client);
                    scanf("%s", message.payload.to_one.message);
                    sendto(socket_fd, &message, sizeof(message), 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
                }
                else {
                    printf("Invalid request type! \n");
                }

                free(request_type_input_buffer);
            } else {
                perror("scanf input");
            }
        }

        request_message_t stop_message = {
                .request_type = STOP
        };

        // Wysylam STOP
        strncpy(stop_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);
        sendto(socket_fd, &stop_message, sizeof(stop_message),
               0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    close(socket_fd);

    return 0;
}
