// Wykorzystanko System V
#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include "common.h"
#include <time.h>
#include <unistd.h>

// Globalna zmienna sluzaca do przydzielania ID kolejnym osoba
int currentClientIndex = 0;

// Tablica do przechowywania kluczy kolejek danych klientow
key_t clients[MAX_CLIENTS_NUMBER];

void handleINIT (Message* messageReceived) {
    key_t msgQueueKey = messageReceived -> clientMsgQueueKey;
    clients[ currentClientIndex ] = msgQueueKey;

    int msgQueueID = msgget ( msgQueueKey, 0666);
    if (msgQueueID < 0 ) { perror("Server - receive message error in handleINIT"); return; }

    Message messageSend;

    // Inicializuje zerem wszystkie miejsca w pamieci
    memset (&messageSend, 0, sizeof(messageSend));

    // Ustawiam nowy indeks dla klienta
    messageSend.clientID = currentClientIndex;
    messageSend.callType = INIT_RESPONSE;
    messageSend.messageType = 1;


    // Wysylam do klienta jego ID
    if ( msgsnd(msgQueueID, &messageSend, sizeof(messageSend) - sizeof(long), 0) < 0 ) {
        perror ("Server - send message error");
        return;
    }

    currentClientIndex++;
}

void handleOTHER (Message* messageReceived) {
    int clientID = messageReceived -> clientID;

    // Wysylam do wszystkich klientow wiadomosc wyslana
    for (int a = 0; a < currentClientIndex; a++) {
        if ( a == clientID ) { continue; }

        Message messageSend = {};
        messageSend.callType = OTHER;
        messageSend.clientID = a;
        messageSend.messageType = 1;
        strcpy (messageSend.message, messageReceived->message);

        // Pozyskuje kolejke danego klienta
        int msgQueueID = msgget ( clients[a], 0666 );

        if ( msgQueueID < 0 ) { perror ("Server - open queue error"); return; }

        if ( msgsnd (msgQueueID, &messageSend, sizeof(messageSend) - sizeof(long), 0) ) {
            perror ("Server - send message error");
            return;
        }
    }
}

void loop ( int msgQueueID ) {
    int err;

    while (1) {
        Message messageReceived = {};

        // Oczekiwanie na wiadomosc od klienta
        err = msgrcv (msgQueueID, &messageReceived, sizeof(messageReceived) - sizeof(long), 1, 0);

        if (err < 0) {
            perror("Server - receive message error in loop function");

            // Usunięcie kolejki komunikatów
            msgctl (msgQueueID, IPC_RMID, NULL);
            return;
        }

        switch ( messageReceived.callType ) {

            // Utworzenie nowego klienta
            case INIT:
                handleINIT (&messageReceived);
                break;

            // Wyslanie wiadomosci przez klienta
            case OTHER:
                handleOTHER (&messageReceived);
                break;
        }
    }
}

void handleSIGINT(int signal) {
    printf ("Deleting server message queue\n");

    // Opoznienie bufora
    fflush (stdout);

    int msgQueueID = msgget ( (key_t) SERVER_ID, 0666 | IPC_CREAT );

    // Usunięcie kolejki komunikatów
    msgctl (msgQueueID, IPC_RMID, NULL);
}

int main () {
    // Obsluga sygnalu
    signal(SIGINT, &handleSIGINT);

    int msgQueueID = msgget ( (key_t) SERVER_ID, 0666 | IPC_CREAT );
    if ( msgQueueID < 0 ) { perror("Server - open queue error"); return 1; }

    loop (msgQueueID);

    // Usuniecie kolejki
    msgctl (msgQueueID, IPC_RMID, NULL);
    return 0;
}