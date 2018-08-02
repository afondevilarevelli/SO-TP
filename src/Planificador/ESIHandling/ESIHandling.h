#ifndef ESI_HANDLING_H
#define ESI_HANDLING_H

#include <pthread.h>
#include <semaphore.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>

#include "../../shared/protocolo.h"
#include "../../shared/mySocket.h"

extern t_log * pLog;

typedef struct {
	t_queue* cola ;
	char* clave;
  int idEsiUsandoClave;
  t_list* esisBloqueadosParaClave;
}cola_clave;

t_list* ListaColas;

t_list* clavesBloqueadas;

typedef enum{NORMAL, ABORTADO, FINALIZADO} ESIState;

typedef struct
{
	ESIState state;
  int socket;
  int id;
  float estimacionAnterior;
  float duracionAnterior;
  int tiempoEsperandoCPU;
}ESI_t;

t_queue * ESIsListos;
t_queue * ESIsBloqueados;
t_queue * ESIsFinalizados;

ESI_t * pESIEnEjecucion;
rtdoEjec_t rtdoEjecucion;
int tiempoParaIterar;

t_list * hilos;

char* claveAVerSiSatisface;
ESI_t* EsiAVerSiEstaBloqueado;
int idAVerSiSatisfaceBloqueo;
ESI_t* esiParaEliminarDeListaColas;

pthread_mutex_t m_listaColas;
pthread_mutex_t m_colaListos;
pthread_mutex_t m_colaBloqueados;
extern pthread_mutex_t m_puedeEjecutar;
sem_t sem_cantESIsListos;
sem_t sem_respuestaESI;
sem_t sem_esperarFinalizarESI;


void terminarHilo( pthread_t * pHilo );
void ESIFinalizado(ESI_t * pESI);
ESI_t * get_and_remove_ESI_by_ID( t_list * ESIs, int id );
void ESIDesconectado( int ESI_ID );
int fueAbortado(ESI_t * pESI);
void abortESI(ESI_t * pESI);
ESI_t * quitarESIDeSuListaActual(int ESI_ID);
void eliminarESIDelSistema( int ESI_ID );
void atenderESI(ESI_t * pESI);
bool is_ESI_ID_equal( ESI_t * pESI, int id );
void recibirNuevosESI(t_config * pConf);
ESI_t * newESI(int socketESI, int id,int rafagaInicial);
void freeESI(ESI_t * pESI);
cola_clave* buscarElementoDeLista(char* clave);
cola_clave* new_cola_clave(char * clave, int idESI);

void atenderConexionEntrante(int listener,int estimacionInicialESI);
bool claveEstaBloqueada(char* clave);
bool condicionSatisfy(void* clave);
void bloquearClaves(t_config* conf);
void sumarTiempoEsperandoCPU(int tiempo);
void closureParaIterar(ESI_t* esi);
bool claveBloqueadaParaESI(char* clave, ESI_t* esi);
bool closureSatisfyBlock(ESI_t* esi);
bool estaBloqueadoPorOtraClave(ESI_t* esi);
bool satisfaceBloqueo(cola_clave* c);
bool satisfaceBloqueoANivelCola(ESI_t* esi);
void borrarEsiDeListaColas(ESI_t* esi);
void closureIterateBorrado(cola_clave* c);
bool condicionEliminarEsi(ESI_t* esi);

#endif
