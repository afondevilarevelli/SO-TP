#ifndef ESI_HANDLING_H
#define ESI_HANDLING_H

#include <pthread.h>
#include <semaphore.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

#include "../../shared/protocolo.h"
#include "../../shared/mySocket.h"

typedef struct
{
  int socket;
  int id;
}ESI_t;

int socketCoord;

t_queue * ESIsListos;
t_queue * ESIsBloqueados;
t_queue * ESIsFinalizados;

ESI_t * pESIEnEjecucion;

t_list * hilos;

sem_t sem_cantESIsListos;
pthread_mutex_t m_ESIEjecutandose;

void terminarHilo( pthread_t * pHilo );
void ESIFinalizado(ESI_t * pESI);
ESI_t * get_and_remove_ESI_by_ID( t_list * ESIs, int id );
void ESIDesconectado( int ESI_ID );
void abortESI( int ESI_ID );
void atenderESI(ESI_t * pESI);
bool is_ESI_ID_equal( ESI_t * pESI, int id );
void recibirNuevosESI(t_config * pConf);
ESI_t * newESI(int socketESI, int id);
void freeESI(ESI_t * pESI);

void atenderConexionEntrante(int listener);

#endif