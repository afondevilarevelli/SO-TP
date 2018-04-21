#include <stdio.h>
#include <collections/list.h>

#include "../shared/testConnection.h"
#include "../shared/mySocket.h"
#include "../shared/protocolo.h"

#define IP_COORD INADDR_ANY
#define PORT_COORD 8000

int registrarConexionEntrante(int listener);
void registrarNuevoESI( int ESI_socket );
ESI_t * new_ESI( int id, int socket );

void procesarSolicitud(void * pData, int sizeOfData);
void procesarSolicitudESI(void * solicitud, int size);
void procesarSolicitudInstancia(void * solicitud, int size);
void procesarSolicitudPlanificador(void * solicitud, int size);

typedef struct
		{
			int id;
			int socket;
			bool connected;
		} ESI_t;
t_list * coord_ESIs;

typedef struct
		{
			int id;
			int socket;
			bool connected;
		} instancia_t;
t_list * coord_instancias;

int main(void)
{
	esis = list_create();
	int listener = listenOn(IP_COORD, PORT_COORD), i;
	int max_fd = listener;
	fd_set master_set, read_set;
	FD_ZERO(&master_set);
	FD_SET(listener, &master_set);

	while(1)
	{
		read_set = master_set;
		select(max_fd + 1, &read_set, NULL, NULL, NULL);

		for(i=0; i <= max_fd; i++)
		{
			if( FD_ISSET( i, &read_set) )
			{
				if( i == listener)
				{
					int new_socket = registrarConexionEntrante(listener);
	
					/*char * question = "Who are you?";
					questionWithBasicProtocol(new_socket, question, strlen(question) +1, (void*)&puts);*/
					FD_SET(new_socket, &master_set);
					if( max_fd < new_socket ) max_fd = new_socket;
				}
				else
				{
					void * data;
					int sizeOfData = recvWithBasicProtocol(i, &data);
	
					if( sizeOfData != 0)
					{
						procesarSolicitud(data, sizeOfData);
						free(data);
					}
					else
					{
						gestionarDesconexion( i );
						puts("DESCONEXION DE CLIENTE");
						FD_CLR(i, &master_set);
						close(i);
					}
				}
			}
		}
	}

}

int registrarConexionEntrante( int listener)
{
	int nuevaConexion = acceptClient(listener);
	
	registrarNuevoESI( nuevaConexion );

	puts("registrada");

	return nuevaConexion;
}

void registrarNuevoESI( int ESI_socket )
{
	//recibir id del ESI

	ESI_t * pESI = new_ESI( nuevo_id, ESI_socket);

	list_add( coord_ESIs, pESI);
}

ESI_t * new_ESI( int id, int socket )
{
	ESI_t * pESI = malloc(sizeof(ESI_t));
	pESI->id = id;
	pESI->socket = socket;
	pESI->connected = true;

	return pESI;
}

void procesarSolicitud( void * pData, int sizeOfData)
{
	tipoDeProceso_t tipoDeProceso = *( (int *)pData );
	void * solicitud = pData + sizeof(tipoDeProceso_t);
	int size = sizeOfData - sizeof(tipoDeProceso_t);

	switch(tipoDeProceso)
	{
		case ESI:
			procesarSolicitudESI(solicitud, size);
			break;

		case INSTANCIA:
			procesarSolicitudInstancia(solicitud, size);
			break;

		case PLANIFICADOR:
			procesarSolicitudPlanificador(solicitud, size);
			break;

		default:
			puts("ERROR: TIPO DE PROCESO NO RECONOCIDO");
			exit(1);
	}
}

void procesarSolicitudESI(void * solicitud, int size)
{
	ESI_t esi;
	esi.id = *((int*)solicitud);
}

void procesarSolicitudInstancia(void * solicitud, int size)
{
	instancia_t instancia;
	instancia.id = *((int*)solicitud);
}

void procesarSolicitudPlanificador(void * solicitud, int size)
{
	//no necesitamos su id pues solo es uno
}

void gestionarDesconexion( int socketDesc )
{
	//buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
	//cambiar el estado de connected a false
}