#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_UNIVERSITARIO 8080
#define PORT_SEGRETERIA 9090

// Funzione per la connessione al server
int connettiServer(const char *server_ip, int port) {
    int sock;
    struct sockaddr_in server_addr;

    // Creazione del socket del client studente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione del socket del client studente");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connessione al server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    return sock;
}

int connettiServerSegreteria() {
    int sock;
    struct sockaddr_in server_addr;

    // Creazione del socket del client studente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione del socket del client studente");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_SEGRETERIA);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server segreteria

    // Connessione al server segreteria
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nella connessione al server segreteria");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void inoltraRichiestaPrenotazioneSegreteria(int client_sock, int richiesta) {
    // Inoltro della richiesta di prenotazione al server segreteria
    send(client_sock, &richiesta, sizeof(richiesta), 0);
    printf("Richiesta di prenotazione inoltrata al server segreteria.\n");
}

int main() {
    // Configurazione per il server universitario
    const char *server_universitario_ip = "127.0.0.1";

    // Connessione al server universitario
    int client_studente_socket_universitario = connettiServer(server_universitario_ip, PORT_UNIVERSITARIO);
    int client_studente_socket_segreteria = connettiServerSegreteria();
    // Menù utente
    int scelta;
    printf("1. Verifica disponibilità esami per un corso\n");
    printf("2. Richiesta prenotazione esame\n");
    printf("3. Chiudi menù.\n");
    printf("Scelta: ");
    scanf("%d", &scelta);

    if (scelta == 1) {
        // Invia al server segreteria il tipo di richiesta per la disponibilità di esami (ad esempio, 1)
        int tipo_richiesta = 1;
        send(client_studente_socket_segreteria, &tipo_richiesta, sizeof(tipo_richiesta), 0);
        printf("Richiesta di disponibilità inviata al server segreteria.\n");
    }
    else if (scelta == 2) {
        // Richiesta al server segreteria per la prenotazione di un esame
        char corso_prenotazione[50];
        printf("Inserisci il corso per la prenotazione: ");
        scanf("%49s", corso_prenotazione);
        
        // Invia la richiesta di prenotazione al server segreteria
        inoltraRichiestaPrenotazioneSegreteria(client_studente_socket_segreteria, scelta);
        
        // Invia il nome del corso al server segreteria
        send(client_studente_socket_segreteria, corso_prenotazione, strlen(corso_prenotazione), 0);
        printf("Richiesta di prenotazione inviata al server segreteria.\n");
    }
    else if (scelta == 3) {
        // Invia al server segreteria il tipo di richiesta per la chiusura (ad esempio, 3)
        int tipo_richiesta = 3;
        send(client_studente_socket_segreteria, &tipo_richiesta, sizeof(tipo_richiesta), 0);
        
        // Chiudi il socket dopo aver inviato la richiesta di chiusura
        close(client_studente_socket_segreteria);
        printf("Chiusura del programma.\n");
    }
    else {
        printf("Scelta non valida. Riprova.\n");
    }

    return 0;
}
