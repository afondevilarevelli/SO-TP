#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "shared/mySocket.h"

#define PORT 8080;
#define IP "127.5.5.4";
#define msg "recibi todo piola";

int main(){
	comunicacionESI(); //creo servidor
	comunicacionCoord(); //creo servidor
	consola();

	return 0;
}

void comunicacionESI(){
	int socket_servidor, new_socket;
	struct sockaddr_in my_addr, their_addr;
	int result;


	socket_servidor = listenOn(inet_addr(IP), PORT);

	new_socket = acceptClient(socket_servidor);

	enviarMensajeESI(socket_servidor);
	recibirMensajeESI(socket_servidor);

	close(socket_servidor);
	close(new_socket);
}

int enviarMensajeESI(int socket_servidor)
{
	int result;

	result = send(socket_servidor, msg, strlen(msg), 0);
		if(result == -1){
			perror("error al enviar datos");
			exit(1);
		}
		return result;
}
int recibirMensajeESI(int socket_servidor)
{
	int result;
	void * buffer[256];
	result = recv(socket_servidor, buffer, sizeof(buffer), 0);
		if(result == -1){
			perror("error al recibir datos");
			exit(1);
		}
	return result;
}


void comunicacionCoord(){
	int listener, new_socket, fdmax, i,;
	struct sockaddr_in my_addr;
	int yes = 1;
	fd_set master, read_fds;
	int result;

	FD_ZERO(master);
	FD_ZERO(read_fds);

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("error al crear el socket");
		exit(1);
	}

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = inet_addr(IP);
	memset(&(my_addr.sin_zero), 8);

	if ( ( bind(listener, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) )  == -1 ) {
		perror("error al bindear");
		exit(1);
	}

	if ( listen(listener,10) == -1) {
		perror("error al escuchar");
	}

	FD_SET(listener, &master);
	fdmax = listener;

	while(1){
		read_fds = master;
		if( (select(fdmax+1, &read_fds, NULL, NULL, NULL)) == -1 ){
			perror("error en el select");
			exit(1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i, read_fds)){
				if(i == listener){
					new_socket = accept(i, (struct sockaddr *)&their_addr, sizeof(struct sockaddr_in));
					FD_SET(new_socket, &master);
					if(new_socket > fdmax){
						fdmax = new_socket;
					}
				}
				else{

					result = recibirMensajeCoordiandor();
					if(result == 0){
						close(i);
					}

					enviarMensajeCoordiandor()

				}
			}
		}
	}
	close(sockfd);
}
int enviarMensajeCoordiandor()
{

	int result;

	 if(result = send(i, msg, strlen(msg), 0)==-1 ){
			perror("error al enviar datos");
			exit(1);
		}
	printf("se enviaron %d bytes", result);
	return result;

}
int recibirMensajeCoordiandor()
{
	int result;

	void buffer[256];
	if( ( result = recv(i, buffer, sizeof(buffer), 0) ) == -1){
			perror("error al recibir datos");
			exit(1);
		}
	if(result == 0){
		return result;
		}
	else{
		printf("se recibieron %d bytes", result);
		}
	return result;

}


