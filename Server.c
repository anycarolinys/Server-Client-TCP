// Aplicação que implementa um protocolo da camada de aplicação em uma arquitetura cliente-servidor na rede
/* Referência: PANTUZA, G. PROGRAMANDO UM PROTOCOLO UTILIZANDO SOCKETS.
 * Disponível em: <https://blog.pantuza.com/artigos/programando-um-protocolo-utilizando-sockets>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Porta do protocolo TCP
#define PORT 4242

// 4096 bytes
#define BUFFER_LENGTH 4096

int main(void) {
    // Structs para representar cliente e servidor
    // 'struct sockaddr_in' lida com endereços de rede
    /*
    * struct sockaddr_in {
    * short            sin_family;
    * unsigned short   sin_port;
    * struct in_addr   sin_addr;
    * char             sin_zero[8];
    * }; 
    */
    struct sockaddr_in client, server;

    // Descritor de arquivo para servidor e cliente
    int serverfd, clientfd;

    char buffer[BUFFER_LENGTH];

    fprintf(stdout, "Server started\n");

    // Criando um socket para suportar IPv4 (AF_INET) e utilizar TCP (SOCK_STREAM)
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        perror("Can't create the server socket:");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Server socket has been created witf file descriptor: %d\n", serverfd);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    // 'memset': função para copiar caracteres em um ponteiro para char
    // 0x0 (hexadecimal) = 0 (decimal)
    memset(server.sin_zero, 0x0, 8);

    /* Lidando com o erro para o caso da porta 4242
    *  estar em uso */
    int yes = 1;

    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Socket options error:");
        return EXIT_FAILURE;
    }

    
    // Conectando o socket ao número da porta definida
    if (bind(serverfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Socket bind error: ");
        return EXIT_FAILURE;
    }

    // Espera conexões vindas dos clientes

    if(listen(serverfd, 1) == -1) {
        perror("Listen error:");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Listening on port %d\n", PORT);

    socklen_t clientLength = sizeof(client);

    if ((clientfd = accept(serverfd, (struct sockaddr *) &client, &clientLength)) == -1){
        perror("Accept error:");
        return EXIT_FAILURE;
    }

    strcpy(buffer, "Hello, client!\n\0");

    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client return...\n");
        // fprintf(stdout, buffer);
        do {
           // Limpando o ponteiro de char buffer
           memset(buffer, 0x0, BUFFER_LENGTH);

            int messageLength;

            if ((messageLength = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) {
                /* Sem o caractere '\0' não é possível no C definir o tamanho de uma string quando está sendo realziado
                um loop sobre a mesma */
                buffer[messageLength - 1] = '\0';
                printf("Client says: %s\n", buffer);

            }

            if (strcmp(buffer, "bye") == 0) {
                send(clientfd, "bye", 3, 0);
            } else {
                send(clientfd, "yep!\n", 5, 0);
            }

        } while (strcmp(buffer, "bye"));
    }

    close(clientfd);
    close(serverfd);

    printf("Connection closed\n\n");

    return EXIT_SUCCESS;
}