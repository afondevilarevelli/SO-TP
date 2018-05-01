#include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
    #include <netdb.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>

    #define PORT 8086 // puerto al que vamos a conectar 
    #define IP INADDR_ANY

    #define MAXDATASIZE 10 // máximo número de bytes que se pueden leer de una vez 

    int main(int argc, char *argv[])
    {
        int numEnviado = 0;
        int sockfd, numbytes;  
        char buffer[32];
        struct sockaddr_in their_addr; // información de la dirección de destino 

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina 
        their_addr.sin_port = htons(PORT);  // short, Ordenación de bytes de la red 
        their_addr.sin_addr.s_addr = htonl(IP);
        memset(&(their_addr.sin_zero),'\0', 8);  // poner a cero el resto de la estructura 

        if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
            perror("connect\n");
            exit(1);
        }

        if ((numbytes=send(sockfd, (void *) &numEnviado, sizeof(numEnviado), 0)) == -1) {
            perror("recv\n");
            exit(1);
        }

        if ((numbytes=recv(sockfd, (void *) buffer, 33, 0)) == -1) {
            perror("send\n");
            exit(1);
        }

        printf("%s\n",buffer);

        close(sockfd);

        return 0;
    } 