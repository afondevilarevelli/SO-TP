#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 8080;
#define IP "127.5.5.4";

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

	socket_servidor = socket(AF_INET,SOCK_STREAM,0);
	if(socket_servidor == -1){
		perror("error al crear socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = inet_addr(IP);
	memset(&(my_addr.sin_zero),'\0',8);

	if ( (bind(socket_servidor, (struct sockaddr *) &my_addr, 0)) == -1 ){
		perror("error al bindear el socket");
		exit(1);
	}

	if ( (listen(socket_servidor,10)) == -1 ){
		perror("error al escuchar");
		exit(1);
	}

	new_socket = accept(socket, (struct sockaddr *)&their_addr, sizeof(struct sockaddr_in));
	if (new_socket == -1 ){
		perror("conexion no aceptada");
		exit(1);
	}

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
	struct hostent *he;
	struct sockaddr_in their_addr; // información de la dirección de destino

	if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("error al crear el socket");
		exit(1);
	}

	their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina
	their_addr.sin_port = htons(PORT);  // short, Ordenación de bytes de la red
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), 8);  // poner a cero el resto de la estructura

	if ( connect(socket_cliente, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1 ) {
		perror("error al conectar");
		exit(1);
	}

	if ( (numbytes = recv(socket_cliente, /*buffer*/, /*tamaño maximo buffer*/, 0)) == -1) {
		perror("error al recibir datos");
		exit(1);
	}

	close(sockfd);
}

