#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT_SEGRETERIA 9090
#define PORT_UNIVERSITARIO 8080
#define MAX_ESAMI 100

// Definizione della struct per gli esami
typedef struct {
    char corso[50];
    char data[20];
} Esame;

int leggiEsamiDaFile(const char* nomeFile, Esame esami[], int* numEsami) {
    FILE* file = fopen(nomeFile, "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return 0;
    }

    int numEsamiLetti = 0;
    while (fscanf(file, "%s %s", esami[numEsamiLetti].corso, esami[numEsamiLetti].data) == 2) {
        numEsamiLetti++;
        if (numEsamiLetti >= MAX_ESAMI) {
            printf("Attenzione: Numero massimo di esami raggiunto.\n");
            break;
        }
    }

    *numEsami = numEsamiLetti;
    fclose(file);
    return 1;
}

// Funzione per connettersi al server universitario e inoltrare la richiesta
void inoltraRichiestaUniversitario(int richiesta) {
    int client_socket_universitario;
    struct sockaddr_in server_universitario_addr;

    // Creazione del socket del client per il server universitario
    if ((client_socket_universitario = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione del socket per il server universitario");
        exit(EXIT_FAILURE);
    }

    server_universitario_addr.sin_family = AF_INET;
    server_universitario_addr.sin_port = htons(PORT_UNIVERSITARIO);
    server_universitario_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server universitario

    // Connessione al server universitario
    if (connect(client_socket_universitario, (struct sockaddr *)&server_universitario_addr, sizeof(server_universitario_addr)) < 0) {
        perror("Errore nella connessione al server universitario");
        exit(EXIT_FAILURE);
    }

    // Inoltro della richiesta al server universitario
    send(client_socket_universitario, &richiesta, sizeof(richiesta), 0);

    close(client_socket_universitario); // Chiude la connessione al server universitario
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    bool shouldExit = false;
    
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
    address.sin_port = htons(PORT_SEGRETERIA);
    
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
    
    // Accettazione delle connessioni in entrata
    while (!shouldExit) {
        printf("\nIn attesa di connessioni...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        // Lettura del tipo di richiesta dal client
        int richiesta;
        valread = read(new_socket, &richiesta, sizeof(richiesta));
        if (valread < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        
        // Gestione della richiesta
        switch (richiesta) {
            case 1: {
                Esame listaEsami[MAX_ESAMI];
                int numEsami = 0;
                
                if (!leggiEsamiDaFile("esami.txt", listaEsami, &numEsami)) {
                    fprintf(stderr, "Errore durante la lettura degli esami dal file.\n");
                    exit(EXIT_FAILURE);
                }
                
                // Richiedi all'utente il nome del corso
                char corsoDesiderato[50];
                printf("Inserisci il nome del corso: ");
                scanf("%s", corsoDesiderato);
                
                // Mostra solo gli esami relativi al corso desiderato
                printf("Esami per il corso '%s':\n", corsoDesiderato);
                int esamiTrovati = 0;
                for (int i = 0; i < numEsami; i++) {
                    if (strcmp(listaEsami[i].corso, corsoDesiderato) == 0) {
                        printf("%s %s\n", listaEsami[i].corso, listaEsami[i].data);
                        esamiTrovati++;
                    }
                }
                
                if (esamiTrovati == 0) {
                    printf("Nessun esame trovato per il corso '%s'.\n", corsoDesiderato);
                }
                break;
            }
            case 2: {
                // Inoltra la richiesta di prenotazione degli studenti al server universitario
                printf("Inoltro della richiesta di prenotazione degli studenti al server universitario...\n");
                inoltraRichiestaUniversitario(richiesta);
                break;
            }
            case 3: {
                printf("Chiusura del server...\n");
                shouldExit = true;
                break;
            }
            default:
                printf("Richiesta non valida.\n");
        }
        close(new_socket); // Chiusura del socket del client
        
        close(server_fd); // Chiusura del socket del server
    }
    
    
    return 0;
}
