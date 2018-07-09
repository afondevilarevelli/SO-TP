#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include "consolaPlanificador.h"
#include "../shared/protocolo.h"
#include "../shared/mySocket.h"
#include <string.h>
#include "algoritmosDePlanificacion.h"

#include "ESIHandling/ESIHandling.h"

void planificarEjecucionESI(t_config * pConf);
void procesarResultadoEjecESI(void * rtdoEjec, int size);

int conectarseACoordinador(t_config * pConf);
void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuertoDePlanificador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuerto(t_config * pConf, int * ip, int * puerto, char * ipKey, char * portKey);
struct tipoPlanificacion obtenerAlgoritmoDePlanificacion(t_config * pConf);

pthread_mutex_t m_puedeEjecutar;

t_log * pLog;

int main(void)
{
	pLog = log_create("planificador.log", "PLANIFICADOR", true, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");

	pthread_mutex_init(&m_puedeEjecutar, NULL);
	sem_init(&sem_cantESIsListos, 0, 0);
	sem_init(&sem_respuestaESI, 0, 0);
	hilos = list_create();
	struct tipoPlanificacion infoAlgoritmo;

	ESIsListos = queue_create();
	ESIsBloqueados = queue_create();
	ESIsFinalizados = queue_create();
	ListaColas = list_create();
	pESIEnEjecucion = NULL;

	log_trace(pLog, "Inicializacion de variables globales completada");

	t_config * pConf = config_create("planificador.config");
	infoAlgoritmo = obtenerAlgoritmoDePlanificacion(pConf);
	log_trace(pLog, "Se obtuvo el algoritmo de planificacion %s", infoAlgoritmo.planificacion==FIFO?"FIFO":infoAlgoritmo.planificacion==SRT?"SRT":infoAlgoritmo.planificacion==SJF?"SJF":"HRRN");
	socketCoord = conectarseACoordinador(pConf);
	log_trace(pLog, "Se conecto al Coordinador en el socket %d", socketCoord);

	pthread_t hiloListener, hiloPlanificacion, hiloConsolaPlanificador;
	pthread_create(&hiloListener, NULL, (void*)&recibirNuevosESI, pConf);
	log_trace(pLog, "Se creo un hilo para recibir ESIs");

	switch(infoAlgoritmo.planificacion){
				case FIFO:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunFIFO, NULL);
					break;
				case SJF:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunSJF, NULL);
					break;
				case SRT:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunSRT, NULL);
					break;
				case HHRR:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunHRRN, NULL);
					break;

				default://rutina de error de algo :)
					printf("No se ha encontrado el Algoritmo de Planificacion a utilizar.\n");
					break;
			}
	log_trace(pLog, "Se creo un hilo para planificar segun %s", infoAlgoritmo.planificacion==FIFO?"FIFO":infoAlgoritmo.planificacion==SJF?"SJF":infoAlgoritmo.planificacion==SRT?"SRT":"HRRN");

	pthread_create(&hiloConsolaPlanificador, NULL, (void*)&consolaPlanificador, NULL);
	log_trace(pLog, "Se creo un hilo para la consola");

	pthread_join(hiloListener, NULL);
	pthread_join(hiloPlanificacion, NULL);
	pthread_join(hiloConsolaPlanificador, NULL);
	list_iterate( hilos, (void *)&terminarHilo );
	log_trace(pLog, "Se destruyeron todos los hilos secundarios");
	queue_destroy_and_destroy_elements(ESIsListos, (void*)&freeESI);
	queue_destroy_and_destroy_elements(ESIsBloqueados, (void*)&freeESI);
	log_trace(pLog, "Se destruyeron las listas globales");

	return 0;
}

/*----CONEXIONES-----*/

int conectarseACoordinador(t_config * pConf)
{
	int ip, puerto;
	obtenerIPyPuertoDeCoordinador(pConf, &ip, &puerto);
	int socket = connectTo(ip, puerto);

	//Le envio al coordinador que tipo de proceso soy
	tProceso proceso = PLANIFICADOR;
	sendWithBasicProtocol(socket, &proceso, sizeof(tProceso));

	return socket;
}

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto)
{
	obtenerIPyPuerto(pConf, ip, puerto, "COORD_IP", "COORD_PUERTO");
}

void obtenerIPyPuertoDePlanificador(t_config * pConf, int * ip, int * puerto)
{
	obtenerIPyPuerto(pConf, ip, puerto, "PLANIF_IP", "PLANIF_PUERTO");
}

void obtenerIPyPuerto(t_config * pConf, int * ip, int * puerto, char * ipKey, char * portKey)
{
	char * strIP = config_get_string_value(pConf, ipKey);
	*ip = inet_addr(strIP);

	//puts(strIP);

	*puerto= htons(config_get_int_value(pConf, portKey));
}

struct tipoPlanificacion obtenerAlgoritmoDePlanificacion(t_config * pConf)
{
	struct tipoPlanificacion plani;

	char * planifString =config_get_string_value(pConf, "ALGORITMO_DE_PLANIFICACION");
	if( !strcmp(planifString, "FIFO") )
	{
		plani.planificacion = FIFO;
		plani.alpha = 0;
  	plani.estimacionInicial = 0;
	}
	else
	{
		plani.alpha= config_get_int_value(pConf, "ALPHA");
  	plani.estimacionInicial= config_get_int_value(pConf, "ESTIMACION_INICIAL");
	}
	return plani;
}
