#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define PORT_UNIVERSITARIO 8080

// Definizione della struct per gli esami
typedef struct {
    char corso[50];
    char data[20];
} Esame;

void scriviEsameSuFile(const char* nomeFile, const Esame* esame) {
    FILE* file = fopen(nomeFile, "a");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s %s\n", esame->corso, esame->data);
    fclose(file);
}

// Funzione per ricevere la richiesta dal server universitario
int riceviRichiestaUniversitario() {
    int server_socket_universitario, client_socket_universitario, richiesta;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creazione del socket
    if ((server_socket_universitario = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Settaggio dell'opzione SO_REUSEADDR per riutilizzare l'indirizzo e la porta
    if (setsockopt(server_socket_universitario, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_UNIVERSITARIO);

    // Binding dell'indirizzo e della porta al socket
    if (bind(server_socket_universitario, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // In ascolto per le connessioni in entrata
    if (listen(server_socket_universitario, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accettazione delle connessioni in entrata
    if ((client_socket_universitario = accept(server_socket_universitario, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Lettura della richiesta dal client
    ssize_t valread;
    valread = read(client_socket_universitario, &richiesta, sizeof(richiesta));

    if (valread < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    close(client_socket_universitario); // Chiude il socket del client

    return richiesta;
}



int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    
    int addrlen = sizeof(address);
    Esame nuovoEsame;

    // Creazione del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Settaggio dell'opzione SO_REUSEADDR per riutilizzare l'indirizzo e la porta
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding dell'indirizzo e della porta al socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // In ascolto per le connessioni in entrata
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Creazione della struct per gli esami e inizializzazione del numero di esami
    
    printf("Vuoi aggiungere un esame? (si/no): ");
    char risposta[3];
    scanf("%s", risposta);
    if (strcmp(risposta, "si") == 0) {
        printf("Inserisci il nome del corso: ");
            scanf("%s", nuovoEsame.corso);

            printf("Inserisci la data dell'esame: ");
            scanf("%s", nuovoEsame.data);

            scriviEsameSuFile("esami.txt", &nuovoEsame);

            printf("Esame aggiunto con successo al file.\n");

    } else if (strcmp(risposta, "no") != 0) {
        printf("Risposta non valida.\n");
    }

    // Accettazione delle connessioni in entrata
    while (1) {
        printf("\nIn attesa di connessioni...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Lettura del tipo di richiesta dal client
        int richiesta;
        valread = (int)read(new_socket, &richiesta, sizeof(richiesta));

        if (valread < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        // Gestione della richiesta
        if (richiesta == 1) {
            // Invia lista degli esami presenti nel database
            printf("Invio lista degli esami presenti nel database alla segreteria...\n");
           
        } else if (richiesta == 2) {
            // Chiamata alla funzione per ricevere la richiesta dal server universitario
            riceviRichiestaUniversitario();
            // Ricezione della prenotazione di un esame
            printf("Richiesta di prenotazione ricevuta.\n");
           
            // Gestione della richiesta dal server universitario...
        } else {
            printf("Richiesta non valida.\n");
        }

        close(new_socket); // Chiusura del socket
    }

    return 0;
}
