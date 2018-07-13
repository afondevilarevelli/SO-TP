#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include "../shared/testConnection.h"
#include "../shared/mySocket.h"
#include "../shared/protocolo.h"

#include "Coord-Instancia/InstanciaHandling.h"
#include "Coord-ESI/ESIHandling.h"
#include "Coord-Planificador/PlanificadorHandling.h"
#include "Coord-Log/coordLog.h"

t_log * pLog, *pOpLog;

pthread_mutex_t m_ESIAtendido;
pthread_mutex_t m_planifAviso;

t_list * claves;
t_list * coord_Insts;
t_list * coord_ESIs;

int socketPlanificador;
int entrySize, entryCant, retardo;

t_list * hilos;

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
void atenderConexionEntrante(int listener);
void terminarHilo( pthread_t * pHilo );

int main(void)
{
	//iniciare todas las variables globales
	pthread_mutex_init(&m_ESIAtendido, NULL);
	pthread_mutex_init(&m_planifAviso, NULL);
	claves = list_create();
	coord_Insts = list_create();
	coord_ESIs = list_create();
	hilos = list_create();

	pLog = log_create("coord.log", "COORDINADOR", true, LOG_LEVEL_TRACE);
	pOpLog = log_create("operaciones.log", "COORDINADOR", false, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");

	unsigned int ip, puerto;
	struct in_addr ip_addr;
	t_config * pConf = config_create("coordinador.config");
	obtenerIPyPuertoDeCoordinador(pConf, &ip, &puerto);
	ip_addr.s_addr = ip;
	log_trace(pLog, "IP (%s) y PUERTO (%d) obtenidos del archivo de configuracion.", inet_ntoa(ip_addr), ntohs(puerto));

	entrySize = config_get_int_value(pConf, "ENTRY_SIZE");
	entryCant = config_get_int_value(pConf, "ENTRY_CANT");
	retardo = config_get_int_value(pConf, "RETARDO");
	//dejare al coordinador escuchar nuevas conexiones a traves del IP y PUERTO indicados
	int listener = listenOn(ip, puerto), i;
	//printf("Coordinador listening on IP:%s y PUERTO:%d\n", inet_ntoa(ip), puerto);
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
	tProceso proceso;
	void * identificacion;

	pthread_t * pNuevoHilo = malloc(sizeof(pthread_t));

	//acepto la conexion
	int nuevaConexion = acceptClient(listener);

	//el proceso conectado me enviara su tipo
	int bytes = recvWithBasicProtocol( nuevaConexion, &identificacion);
	if(!bytes)
	{
		log_error(pLog, "Conexion en socket %d perdida antes de ser identificada", nuevaConexion);
		exit(1);
	}

	proceso = *( (tProceso *)identificacion );
	log_trace(pLog, "Se conecto un proceso de tipo %s", proceso == ESI?"ESI":proceso == PLANIFICADOR?"PLANIFICADOR":proceso == INSTANCIA?"INSTANCIA":"desconocido");
	//iniciare un hilo para atender al proceso segun su tipo
	switch(proceso)
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
			log_error(pLog, "Proceso de tipo desconocido");
			exit(1);
	}

	//agregare el hilo a la lista global de hilos del coordinador
	list_add(hilos, (void*)pNuevoHilo);
}

/*-------------GENERAL--------------*/

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto)
{
	char * strIP = config_get_string_value(pConf, "COORD_IP");
	*ip = inet_addr(strIP);

	//puts(strIP);

	*puerto= htons(config_get_int_value(pConf, "COORD_PUERTO"));

}


void terminarHilo( pthread_t * pHilo )
{
	pthread_join(*pHilo, NULL);
}
