#include "ESIHandling.h"
/*--------NUEVOS ESIS--------*/

void finalizarESI(ESI_t * pESI)
{
  pESI->state = FINALIZADO;
}

int fueFinalizado(ESI_t * pESI)
{
  return pESI->state == FINALIZADO;
}

void atenderESI(ESI_t * pESI)
{
  log_trace(pLog, "Atendiendo ESI de ID=%d", pESI->id);

  while(!fueAbortado(pESI) && !fueFinalizado(pESI))
  {
    int size;
    void * rtdoEjec;

    log_trace(pLog, "A la espera de respuesta del ESI %d", pESI->id);
    size = recvWithBasicProtocol(pESI->socket, &rtdoEjec);

    if( size  ) // SI NO SE DESCONECTO
    {
      rtdoEjecucion = *((rtdoEjec_t*)rtdoEjec);
      log_debug(pLog, "Se recibio el rtdoEjec = %s", rtdoEjecucion==SUCCESS?"SUCCESS":rtdoEjecucion==FAILURE?"FAILURE":rtdoEjecucion==FIN_DE_EJECUCION?"FIN DE EJECUCION":rtdoEjecucion==DISCONNECTED?"DESCONECTADO":rtdoEjecucion==NO_HAY_INSTANCIAS_CONECTADAS?"NO HAY INSTANCIAS CONECTADAS":rtdoEjecucion==ABORTED?"ABORTADO":"SENTENCIA");
      sem_post(&sem_respuestaESI);
    }
    else
    {
      ESIDesconectado( pESI->id );
      rtdoEjecucion = DISCONNECTED;
      sem_post(&sem_respuestaESI);
      eliminarESIDelSistema(pESI->id);
      break;
    }
  }

  if(fueAbortado(pESI)){ 
    eliminarESIDelSistema(pESI->id); }
  else{ 
    finalizarESI(pESI);
    log_warning(pLog, "El ESI con ID = %d finalizo y no atendera mas solicitudes", pESI->id);
  }
}

void abortESI(ESI_t * pESI)
{
  pESI->state = ABORTADO;
  orden_t orden = ABORTAR;
  log_error(pLog, "El ESI con ID = %d, fue abortado\n", pESI->id);
  sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
}

int fueAbortado(ESI_t * pESI)
{
  return pESI->state == ABORTADO;
}

void ESIDesconectado( int ESI_ID )
{
  log_error(pLog, "El ESI con ID = %d, se desconecto\n", ESI_ID);
}

ESI_t * quitarESIDeSuListaActual(int ESI_ID)
{
  ESI_t * pESI = NULL;
  if( pESIEnEjecucion && is_ESI_ID_equal(pESIEnEjecucion, ESI_ID) )
    pESI = pESIEnEjecucion;

  if(!pESI)
  {
    pthread_mutex_lock(&m_colaListos);
    pESI = get_and_remove_ESI_by_ID( ESIsListos->elements, ESI_ID);
    pthread_mutex_unlock(&m_colaListos);
    if(pESI) sem_wait(&sem_cantESIsListos);
  }

  if(!pESI)
  {
    pthread_mutex_lock(&m_colaBloqueados);
    pESI = get_and_remove_ESI_by_ID( ESIsBloqueados->elements, ESI_ID);
    pthread_mutex_unlock(&m_colaBloqueados); }

    if(!pESI){
      pESI = get_and_remove_ESI_by_ID( ESIsFinalizados->elements, ESI_ID); }


  return pESI;
}

void eliminarESIDelSistema( int ESI_ID )
{
  ESI_t * pESI = quitarESIDeSuListaActual(ESI_ID);


  freeESI(pESI);
  log_warning(pLog, "El ESI con ID = %d, fue eliminado del sistema\n", ESI_ID);
}

ESI_t * get_and_remove_ESI_by_ID( t_list * ESIs, int id )
{
  t_link_element * pAct = ESIs->head , * pPrev = NULL;
  ESI_t * pESI;

  while( pAct != NULL )
  {
    pESI = (ESI_t *)(pAct->data);

    if( is_ESI_ID_equal( pESI, id) )
    {
      if(!pPrev)
        ESIs->head = pAct->next;
      else
        pPrev->next = pAct->next;

      return pESI;
    }
    pPrev = pAct;
    pAct = pAct->next;
  }

  return NULL;
}

bool is_ESI_ID_equal( ESI_t * pESI, int id )
{
  return pESI->id == id;
}


ESI_t * newESI( int socket, int id, int rafagaInicial)
{
  ESI_t * pESI = malloc(sizeof(ESI_t));

  pESI->socket = socket;
  pESI->id = id;
  pESI->estimacionAnterior= rafagaInicial;
  pESI->duracionAnterior = rafagaInicial;
  pESI->state = NORMAL;
  pESI->tiempoEsperandoCPU = 0;

  return pESI;
}

void freeESI(ESI_t * pESI)
{
  free(pESI);
}

cola_clave* buscarElementoDeLista(char* clave){//busca de mi ListaColas la que se identifica con la clave
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if (!strcmp(c -> clave, clave)){
			return c;
		}
	p = p -> next ;
	}
	return NULL ;
}

cola_clave* new_cola_clave(char * clave, int idESI)
{
  cola_clave * pCClave = malloc(sizeof(cola_clave));
  pCClave->clave = malloc(strlen(clave)+1);
  strcpy(pCClave->clave, clave);
  pCClave->idEsiUsandoClave = idESI;
  pCClave->cola = queue_create();

  return pCClave;
}

bool claveEstaBloqueada(char* clave){
  claveAVerSiSatisface = clave;
  return list_any_satisfy(clavesBloqueadas, (void*) condicionSatisfy);
}

bool condicionSatisfy(char* clave){
    return clave == claveAVerSiSatisface;
}

void bloquearClaves(t_config* conf){
	int i = 1;
	int posicion = 14;
	char claveBloqueada[] = "claveBloqueadaX";
	claveBloqueada[posicion] = i + '0';
	char* clave = config_get_string_value(conf, claveBloqueada);
	while(clave != NULL){
		list_add(clavesBloqueadas, (void*)clave );
		i++;
		claveBloqueada[posicion] = i + '0';	
		char* clave = config_get_string_value(conf, claveBloqueada);
	}
}

/*----------CONEXIONES---------*/

void recibirNuevosESI(t_config * pConf)
{
  int ip, puerto;
  obtenerIPyPuertoDePlanificador(pConf, &ip, &puerto);

  int listener = listenOn(ip, puerto);

  fd_set masterSet, readSet;
  FD_ZERO(&masterSet);
  FD_SET(listener, &masterSet);

  int maxFD = listener, i;
  while(1)
  {
    readSet = masterSet;
    select(maxFD + 1, &readSet, NULL, NULL, NULL);

    atenderConexionEntrante(listener,config_get_int_value(pConf, "ESTIMACION_INICIAL"));
  }
}

void atenderConexionEntrante(int listener, int estimacionInicialESI)
{
  int newSock = acceptClient(listener);
  void * pID = NULL;
  if(recvWithBasicProtocol(newSock, &pID))
  {
    ESI_t * pESI = newESI(newSock, *((int*)pID),estimacionInicialESI );
    pthread_mutex_lock(&m_colaListos);
    queue_push(ESIsListos, pESI);
    pthread_mutex_unlock(&m_colaListos);
    sem_post(&sem_cantESIsListos);

    pthread_t hiloESI;
    pthread_create(&hiloESI, NULL, (void*)&atenderESI, (void*)pESI);
    //agregare el hilo a la lista global de hilos del coordinador
    list_add(hilos, (void*)&hiloESI);

    free(pID);
  }
  else{
    perror("no se pudo atender la conexion entrante");
    exit(1);
  }
}

void terminarHilo( pthread_t * pHilo )
{
  pthread_join(*pHilo, NULL);
}
