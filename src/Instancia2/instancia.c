#include <commons/config.h>

#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

void obtenerIPyPuertoDeCoordinador(int * ip, int * puerto);

int main(void)
{

	int coord_socket = conectarseACoordinador();

	while(1)
	{
		void * ordenDeAcceso;
		int size = recvWithBasicProtocol(coord_socket, &ordenDeAcceso);
		printf("size=%d\n", size);

		int aviso = SOLICITUD_ESI_ATENDIENDOSE;
		sendWithBasicProtocol(coord_socket, &aviso, sizeof(int));

		tBuffer * buffSent = makeBuffer(ordenDeAcceso, size);
		ESISentenciaParseada_t sent;
		sent.operacion = readIntFromBuffer(buffSent);
		sent.clave = readStringFromBuffer(buffSent);
		if(sent.operacion == SET)
			sent.valor = readStringFromBuffer(buffSent);
		else
			sent.valor = NULL;

		freeBuffer(buffSent);

		printf("op=%d, clave=%s, valor=%s\n", sent.operacion, sent.clave, sent.valor?sent.valor:"No corresponde");
		rtdoEjec_t rtdo = SUCCESS;
		sendWithBasicProtocol(coord_socket, &rtdo, sizeof(rtdoEjec_t));
	}

	return 0;
}

int conectarseACoordinador(void)
{
	int ip, puerto;
	obtenerIPyPuertoDeCoordinador(&ip, &puerto);
	int coord_socket = connectTo(ip, puerto);

	tProceso tipoDeProceso = INSTANCIA;
	int instancia_ID = 42;

	sendWithBasicProtocol( coord_socket, &tipoDeProceso, sizeof(tProceso));
	sendWithBasicProtocol( coord_socket, &instancia_ID, sizeof(int));

	return coord_socket;
}

void obtenerIPyPuertoDeCoordinador(int * ip, int * puerto)
{
	t_config * pConf = config_create("instancia.config");

	char * strIP = config_get_string_value(pConf, "COORD_IP");
	*ip = inet_addr(strIP);

	//puts(strIP);

	*puerto= htons(config_get_int_value(pConf, "COORD_PUERTO"));
}
