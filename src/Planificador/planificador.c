#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "consolaPlanificador.h"
#include "../shared/protocolo.h"
#include "../shared/mySocket.h"
#include <string.h>
#include "algoritmosDePlanificacion.h"

#include "ESIHandling/ESIHandling.h"
#include "CoordHandling/CoordHandling.h"

int conectarseACoordinador(t_config * pConf);
void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuertoDePlanificador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuerto(t_config * pConf, int * ip, int * puerto, char * ipKey, char * portKey);
void destruirListaColas();
void elementoDestructorLista(cola_clave* c);
void elementoDestructorClaves(char* c);

pthread_mutex_t m_puedeEjecutar;

t_log * pLog;

int socketCoord;

int main(void)
{
	/*
	//INICIO_PRUEBAS
	ESIsListos = queue_create();
	pESIEnEjecucion = NULL;
	tipoPlanificacion = malloc(sizeof(tipoPlanif));
	t_config * pConf = config_create("planificador.config");
	obtenerEstructuraDePlanificacion(pConf);
	tipoPlanif estructura = *(tipoPlanificacion);

	// ALPHA = 30 
	ESI_t esi1, esi2, esi3;
    esi1.state = 0;
    esi1.socket = 153;
    esi1.id = 1;
    esi1.estimacionAnterior = 5.;
    esi1.duracionAnterior = 7.;
    esi1.tiempoEsperandoCPU = 5;

	esi2.state = 0;
    esi2.socket = 122;
    esi2.id = 2;
    esi2.estimacionAnterior = 5;
    esi2.duracionAnterior = 3;
    esi2.tiempoEsperandoCPU = 10;

	esi3.state = 0;
    esi3.socket = 166;
    esi3.id = 3;
    esi3.estimacionAnterior = 2;
    esi3.duracionAnterior = 2;
    esi3.tiempoEsperandoCPU = 14;

	queue_push(ESIsListos, &esi2);
	queue_push(ESIsListos, &esi3);
	queue_push(ESIsListos, &esi1);

	float est1 = algoritmoDeEstimacionProximaRafaga(&esi1);
	float est2 = algoritmoDeEstimacionProximaRafaga(&esi2);
	float est3 = algoritmoDeEstimacionProximaRafaga(&esi3);
    float ratio1 = calcularRatio(&esi1); 
	float ratio2 = calcularRatio(&esi2);
	float ratio3 = calcularRatio(&esi3); 

	ESI_t* esiElegido = obtenerEsiAEjecutarSegunHRRN();
	//printf("\nestimacion1 = %f\n",est1);
	//printf("estimacion2 = %f\n",est2);
	//printf("estimacion3 = %f\n",est3);
	printf("ratio1 = %f\n",ratio1);
	printf("ratio2 = %f\n",ratio2);
	printf("ratio3 = %f\n",ratio3);

	printf("el esi elegido fue el ESI con id = %d\n",esiElegido->id);

	//FIN_PRUEBAS
	*/
	

	
	pLog = log_create("planificador.log", "PLANIFICADOR", true, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");

	pthread_mutex_init(&m_puedeEjecutar, NULL);
	pthread_mutex_init(&m_listaColas, NULL);
	pthread_mutex_init(&m_colaListos, NULL);
	pthread_mutex_init(&m_colaBloqueados, NULL);
	sem_init(&sem_cantESIsListos, 0, 0);
	sem_init(&sem_respuestaESI, 0, 0);
	sem_init(&sem_esperarFinalizarESI, 0, 0);
	hilos = list_create();

	ESIsListos = queue_create();
	ESIsBloqueados = queue_create();
	ESIsFinalizados = queue_create();
	ListaColas = list_create();
	clavesBloqueadas = list_create();
	pESIEnEjecucion = NULL;
	claveAVerSiSatisface = malloc(sizeof(char)*40);
	claveAVerSiSatisface = NULL;


	log_trace(pLog, "Inicializacion de variables globales completada");

	tipoPlanificacion = malloc(sizeof(tipoPlanif));
	t_config * pConf = config_create("planificador.config");
	obtenerEstructuraDePlanificacion(pConf);
	log_trace(pLog, "Se obtuvo el algoritmo de planificacion %s", tipoPlanificacion->planificacion==FIFO?"FIFO":tipoPlanificacion->planificacion==SRT?"SRT":tipoPlanificacion->planificacion==SJF?"SJF":"HRRN");
	socketCoord = conectarseACoordinador(pConf);
	log_trace(pLog, "Se conecto al Coordinador en el socket %d", socketCoord);

	pthread_t hiloListener, hiloPlanificacion, hiloConsolaPlanificador, hiloCoordinador;
	pthread_create(&hiloListener, NULL, (void*)&recibirNuevosESI, pConf);
	log_trace(pLog, "Se creo un hilo para recibir ESIs");

	pthread_create(&hiloCoordinador, NULL, (void*)&atenderCoordinador, (void*)socketCoord);
	log_trace(pLog, "Se creo un hilo para colaborar con el Coordinador");

	switch(tipoPlanificacion->planificacion){
				case FIFO:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunFIFO, NULL);
					break;
				case SJF:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunSJF, NULL);
					break;
				case SRT:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunSRT, NULL);
					break;
				case HRRN:
					pthread_create(&hiloPlanificacion, NULL, (void*)&planificarSegunHRRN, NULL);
					break;

				default://rutina de error de algo :)
					printf("No se ha encontrado el Algoritmo de Planificacion a utilizar.\n");
					break;
			}
	log_trace(pLog, "Se creo un hilo para planificar segun %s", tipoPlanificacion->planificacion==FIFO?"FIFO":tipoPlanificacion->planificacion==SJF?"SJF":tipoPlanificacion->planificacion==SRT?"SRT":"HRRN");

	pthread_create(&hiloConsolaPlanificador, NULL, (void*)&consolaPlanificador, pConf);
	log_trace(pLog, "Se creo un hilo para la consola");

	pthread_join(hiloCoordinador, NULL);
	pthread_join(hiloListener, NULL);
	pthread_join(hiloPlanificacion, NULL);
	pthread_join(hiloConsolaPlanificador, NULL);
	list_iterate( hilos, (void *)&terminarHilo );
	log_trace(pLog, "Se destruyeron todos los hilos secundarios");
	queue_destroy_and_destroy_elements(ESIsListos, (void*)&freeESI);
	queue_destroy_and_destroy_elements(ESIsBloqueados, (void*)&freeESI);
	queue_destroy_and_destroy_elements(ESIsFinalizados, (void*)&freeESI);
	list_destroy_and_destroy_elements(clavesBloqueadas, (void *)&elementoDestructorClaves);
	destruirListaColas();
	log_trace(pLog, "Se destruyeron las listas globales"); 
	
	free(tipoPlanificacion);
	free(claveAVerSiSatisface);
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

void destruirListaColas(){
	list_destroy_and_destroy_elements(ListaColas, (void *)&elementoDestructorLista);
}

void elementoDestructorLista(cola_clave* c){
	free(c->clave);
	queue_destroy_and_destroy_elements(c->cola,(void *)&freeESI);
}

void elementoDestructorClaves(char* c){
	free(c);
}