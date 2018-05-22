#include <stdio.h>
#include <pthread.h>
#include <commons/collections/list.h>

#include "../shared/testConnection.h"
#include "../shared/mySocket.h"
#include "../shared/protocolo.h"

#include "Coord-Instancia/InstanciaHandling.h"
#include "Coord-ESI/ESIHandling.h"
#include "Coord-Planificador/PlanificadorHandling.h"

#define IP_COORD INADDR_ANY
#define PORT_COORD 8000

t_list * hilos;

void atenderConexionEntrante(int listener);
void terminarHilo( pthread_t * pHilo );

int main(void)
{
	//iniciare todas las variables globales de listas
	coord_Insts = list_create();
	coord_ESIs = list_create();
	hilos = list_create();

	//dejare al coordinador escuchar nuevas conexiones a traves del IP y PUERTO indicados
	int listener = listenOn(IP_COORD, PORT_COORD), i;
	int max_fd = listener;
	fd_set master_set, read_set;
	FD_ZERO(&master_set);
	FD_SET(listener, &master_set);

	//lo dejare atender las conexiones entrantes que detecta el listener
	while(1)
	{
		read_set = master_set;
		select(listener  + 1, &read_set, NULL, NULL, NULL);

		atenderConexionEntrante(listener);
	}

	//cuando salga del ciclo de atender conexiones, esperara a que terminen todos los hilos antes de cerrar el programa
	list_iterate( hilos, (void *)&terminarHilo );
}

void atenderConexionEntrante( int listener)
{
	tipoDeProceso_t tipoDeProceso;
	void * identificacion;

	pthread_t * pNuevoHilo = malloc(sizeof(pthread_t));

	//acepto la conexion
	int nuevaConexion = acceptClient(listener);

	//el proceso conectado me enviara su tipo
	recvWithBasicProtocol( nuevaConexion, &identificacion);
	tipoDeProceso = *( (tipoDeProceso_t *)identificacion );

	//iniciare un hilo para atender al proceso segun su tipo
	switch(tipoDeProceso)
	{
		case ESI:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderESI, (void *)nuevaConexion );
			break;
		case INSTANCIA:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderInstancia, (void *)nuevaConexion );
			break;
		case PLANIFICADOR:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderPlanificador, (void *)nuevaConexion );
			break;
		default:
			puts("Tremendo error");
			exit(1);
	}

	//agregare el hilo a la lista global de hilos del coordinador
	list_add(hilos, (void*)pNuevoHilo);
}

/*-------------GENERAL--------------*/

void terminarHilo( pthread_t * pHilo )
{
	pthread_join(*pHilo, NULL);
}
