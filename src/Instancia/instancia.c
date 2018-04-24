#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

#define COORD_IP INADDR_ANY
#define COORD_PORT 8000

int main(void)
{
	int coord_socket = connectTo(COORD_IP, COORD_PORT);

	tipoDeProceso_t tipoDeProceso = INSTANCIA;
	int instancia_ID = 42;	

	sendWithBasicProtocol( coord_socket, &tipoDeProceso, sizeof(tipoDeProceso_t));
	sendWithBasicProtocol( coord_socket, &instancia_ID, sizeof(int));

	return 0;	
}