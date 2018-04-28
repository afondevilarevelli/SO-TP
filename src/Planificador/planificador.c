#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "shared/protocolo.h"
#include "shared/mySocket.h"

#define PORT 8080; 
#define IP "127.5.5.4";
#define msgCoord "Hola Coordinador";
#define msgESI "Hola ESI";

int main(){
/*	comunicacionESI(); //creo servidor
	comunicacionCoord(); //creo servidor
	consola();
*/
	int listener, new_socket;
	int fdmax;
	int result;

	listener = listenOn(inet_addr(IP), PORT);
	FD_S *master, *read_fds;

	FD_ZERO(master);
	FD_ZERO(read_fds);

	FD_SET(listener, &master);
	fdmax = listener;

	while(1){
		int i;
		read_fds = master;
		if( (select(fdmax+1, &read_fds, NULL, NULL, NULL)) == -1 ){
			perror("error en el select");
			exit(1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i, read_fds)){
				if(i == listener){
					new_socket = acceptClient(i);	
					FD_SET(new_socket, &master);
					if(new_socket > fdmax){
						fdmax = new_socket;
					}
				}
				else{
					int bufferHeader; 
					if( ( result = recv(i, bufferHeader, sizeof(bufferHeader), 0) ) == -1){ //hay que verificar si es ESI o COORDINADOR
						perror("error al recibir datos");
						exit(1);
					}
					if(result == 0){
						close(i);
					}
					else{
						if(bufferHeader == COORDINADOR){//se conectó el coordinador
							printf("se conecto el coordinador\n");
							if( ( result = send(i, msgCoord, strlen(msgCoord), 0) ) == -1 ){
								perror("error al enviar datos");
								exit(1);
							}							
						}
						else{// se conectó el esi
							printf("se conecto el ESI\n");
							if( ( result = send(i, msgESI, strlen(msgESI), 0) ) == -1 ){
								perror("error al enviar datos");
								exit(1);
							}	
						}
					}
				}
			}
		}
	}
	close(listener);
	
	return 0;
}

int enviarEjecutarProxSentenciaESI(int socket_servidor)
{
	int result;
   int mensaje =1;
	result = send(socket_servidor, mensaje, strlen(msg), 0);
		if(result == -1){
			perror("error al enviar datos");
			exit(1);
		}
		return result;
}
int recibirResultadoDeEjecucionESI(int socket_servidor)
{
	int result;
	void * buffer[256];
	result = recv(socket_servidor, buffer, sizeof(buffer), 0);
	if(result ==6){puts("funciono 6");}//Verificacion de q funciona
		if(result == -1){
			perror("error al recibir datos");
			exit(1);
		}
	return result;
}