#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Porta padrão para o protocolo TCP
#define PORT 4242

// Tamanho dos buffers de 4096 bytes
#define BUFFER_LENGTH 4096

// Endereço de loopback
#define SERVER_ADDR "127.0.0.1"

int main(int argc, char *argv[]) {
    struct sockaddr_in server;

    // Descritor de arquivo para o socket do cliente
    int clientfd;

    int serverLength = sizeof(server);

    int messageLength;

    char buffer_in[BUFFER_LENGTH];
    char buffer_out[BUFFER_LENGTH];

    fprintf(stdout, "Client started...\n");

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        perror("Can't create the client socket:");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Client socket has been created with file descriptor: %d\n", clientfd);

    // Definindo as propriedades do servidor a ser conectado
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    memset(server.sin_zero, 0x0, 8);

    // Tenta estabelecer conexão com o servidor
    if (connect(clientfd, (struct sockaddr *) &server, serverLength) == -1) {
        perror("Can't connect to server\n");
        return EXIT_FAILURE;
    }

    // Recebe a mensagem de apresentação de boas vindas (handshake)
    if ((messageLength = recv(clientfd, buffer_in, BUFFER_LENGTH, 0)) > 0) {
        buffer_in[messageLength + 1] = '\0';
        fprintf(stdout, "Server says: %s\n", buffer_in);
    }

    while (true) {
        // Limpando os buffers de entrada e saída
        memset(buffer_in, 0x0, BUFFER_LENGTH);
        memset(buffer_out, 0x0, BUFFER_LENGTH);

        fprintf(stdout, "Send a message to the server: ");
        fgets(buffer_out, BUFFER_LENGTH, stdin);

        // Envia a mensagem para o servidor através do socket
        send(clientfd, buffer_out, strlen(buffer_out), 0);

        // Recebe a mensagem do servidor
        messageLength = recv(clientfd, buffer_in, BUFFER_LENGTH, 0);
        printf("Server answered: %s\n", buffer_in);
    
        if (strcmp(buffer_in, "bye") == 0)
            break;
    }

    close(clientfd);

    fprintf(stdout, "Connection closed!\n\n");

    return EXIT_SUCCESS;

}