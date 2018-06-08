#include <pthread.h>
#include <commons/config.h>
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

int main(void)
{
	sem_init(&sem_cantESIsListos, 0, 0);
	sem_init(&sem_respuestaESI, 0, 0);
	hilos = list_create();
	struct tipoPlanificacion infoAlgoritmo;
	rtdoEjecucion = malloc(sizeof(rtdoEjec_t));

	ESIsListos = queue_create();
	ESIsBloqueados = queue_create();
	ESIsFinalizados = queue_create();
	ListaColas = list_create();

	t_config * pConf = config_create("planificador.config");
	infoAlgoritmo = obtenerAlgoritmoDePlanificacion(pConf);
	socketCoord = conectarseACoordinador(pConf);

	pthread_t hiloListener, hiloPlanificacion, hiloConsolaPlanificador;
	pthread_create(&hiloListener, NULL, (void*)&recibirNuevosESI, pConf);
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
	pthread_create(&hiloConsolaPlanificador, NULL, (void*)&consolaPlanificador, NULL);

	pthread_join(hiloListener, NULL);
	pthread_join(hiloPlanificacion, NULL);
	pthread_join(hiloConsolaPlanificador, NULL);
	list_iterate( hilos, (void *)&terminarHilo );
	queue_destroy_and_destroy_elements(ESIsListos, (void*)&freeESI);
	queue_destroy_and_destroy_elements(ESIsBloqueados, (void*)&freeESI);

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

	plani.planificacion= config_get_int_value(pConf, "ALGORITMO_DE_PLANIFICACION");
	if(plani.planificacion == FIFO){
		plani.alpha = 0;
  	    plani.estimacionInicial = 0;
	}
	else{ 
		plani.alpha= config_get_int_value(pConf, "ALPHA");
  	    plani.estimacionInicial= config_get_int_value(pConf, "ESTIMACION_INICIAL");
	}
	return plani;
}