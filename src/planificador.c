#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "shared/mySocket.h"

#define PORT 8080;
#define IP "127.5.5.4";

#define PORT_COORD 8081
#define IP_COORD "127.0.1.3"

int main(){
	comunicacionESI(); //creo servidor
	comunicacionCoord(); //creo cliente
	consola();

	return 0;
}

void comunicacionESI(){
	int socket_servidor, new_socket;
	struct sockaddr_in my_addr, their_addr;
	int result;

	socket_servidor = listenOn(inet_addr(IP), PORT);

	new_socket = acceptClient(socket_servidor);

	result = recv(socket_servidor, /*buffer*/, /*tamaño maximo del buffer*/, 0);
	if(result == -1){
		perror("error al recibir datos");
		exit(1);
	}

	result = send(socket_servidor,/*puntero a lo que quiero enviar*/, sizeof(/*lo que quiero enviar*/), 0);
	if(result == -1){
		perror("error al enviar datos");
		exit(1);
	}

	close(socket_servidor);
	close(new_socket);
}

void comunicacionCoord(){
	int socket_cliente, numbytes;
	struct sockaddr_in their_addr; // información de la dirección de destino

	socket_cliente = connectTo(inet_addr(IP_COORD), PORT_COORD);
	
	if ( (numbytes = recv(socket_cliente, /*buffer*/, /*tamaño maximo buffer*/, 0)) == -1) {
		perror("error al recibir datos");
		exit(1);
	}

	close(sockfd);
}

