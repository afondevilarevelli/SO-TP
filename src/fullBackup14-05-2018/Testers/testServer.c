#include "stdio.h"
#include "string.h"
#include "../shared/mySocket.h"
#include "../shared/testConnection.h"

#define IP INADDR_ANY
#define PORT 8000

int main(void)
{
	int listener = listenOn(IP, PORT);

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(listener, &fds);

	select( listener+1, &fds, NULL, NULL, NULL);
	puts("SOLICITUD DE CONEXION DETECTADA");

	int client = acceptClient(listener);

	char * question = "Who is it?";
	questionWithBasicProtocol(client, question, strlen(question) + 1, (void*)&puts);

	return 0;
}