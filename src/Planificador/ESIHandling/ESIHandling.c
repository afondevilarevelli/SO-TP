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
      log_debug(pLog, "Se recibio el rtdoEjec = %d", rtdoEjecucion);
      sem_post(&sem_respuestaESI);
    }
    else
    {
      ESIDesconectado( pESI->id );
      rtdoEjecucion = DISCONNECTED;
      sem_post(&sem_respuestaESI);
      break;
    }
  }

  if(fueAbortado(pESI))
    eliminarESIDelSistema(pESI->id);
  else
    log_warning(pLog, "El ESI %d finalizo y no atendera mas solicitudes", pESI->id);
}

void abortESI(ESI_t * pESI)
{
  pESI->state = ABORTADO;
  orden_t orden = ABORTAR;
  log_error(pLog, "El ESI de id %d, fue abortado\n", pESI->id);
  sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
}

int fueAbortado(ESI_t * pESI)
{
  return pESI->state == ABORTADO;
}

void ESIDesconectado( int ESI_ID )
{
  log_error(pLog, "El ESI de id %d, se desconecto\n", ESI_ID);
}

ESI_t * quitarESIDeSuListaActual(int ESI_ID)
{
  ESI_t * pESI = NULL;
  if( is_ESI_ID_equal(pESIEnEjecucion, ESI_ID) )
    pESI = pESIEnEjecucion;

  if(!pESI)
  {
    pESI = get_and_remove_ESI_by_ID( ESIsListos->elements, ESI_ID);
    if(pESI) sem_wait(&sem_cantESIsListos);
  }

  if(!pESI)
  {
    pESI = get_and_remove_ESI_by_ID( ESIsBloqueados->elements, ESI_ID);

    if(!pESI)
      pESI = get_and_remove_ESI_by_ID( ESIsFinalizados->elements, ESI_ID);
  }

  return pESI;
}

void eliminarESIDelSistema( int ESI_ID )
{
  ESI_t * pESI = quitarESIDeSuListaActual(ESI_ID);


  freeESI(pESI);
  log_warning(pLog, "El ESI de id %d, fue eliminado del sistema\n", ESI_ID);
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
  pESI-> duracionAnterior = rafagaInicial;
  pESI->state = NORMAL;

  return pESI;
}

void freeESI(ESI_t * pESI)
{
  free(pESI);
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
    queue_push(ESIsListos, pESI);
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
