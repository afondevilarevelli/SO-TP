#include <stdlib.h>

#include "../shared/mySocket.h"
#include "../shared/testConnection.h"

int main(int count, char * args[])
{
	if( count != 2)
	{
		puts("Hay que ingresar un PUERTO");
		exit(1);
	}

	printf("INICIANDO CONEXION CON PUERTO %d\n", atoi(args[1]));
	int socket = connectTo(inet_addr("1.0.0.127"), atoi(args[1]));
	puts("CONEXION EXITOSA");
	char * answer = "I'm Mister testClient";
	answerWithBasicProtocol(socket, answer, strlen(answer) +1, (void*)&puts);

	return 0;
}
