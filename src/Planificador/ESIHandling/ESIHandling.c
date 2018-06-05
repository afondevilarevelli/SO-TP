#include "ESIHandling.h"
/*--------NUEVOS ESIS--------*/

void atenderESI(ESI_t * pESI)
{
  printf("Atendiendo ESI de ID=%d\n", pESI->id);

  while(1)
  {
    int size;
    void * rtdoEjec;

    size = recvWithBasicProtocol(pESI->socket, &rtdoEjec);

    if( size ) // SI NO SE DESCONECTO
    {
      if( ( size == sizeof(int) ) && ((*((int*)rtdoEjec)) == FIN_DE_EJECUCION) )
      {
        break;
      }
      else
      {
        procesarResultadoEjecESI( rtdoEjec, size);
        free(rtdoEjec);
      }
    }
    else
    {
      ESIDesconectado( pESI->id );
      return;
    }

  }

  ESIFinalizado(pESI);
}

void ESIFinalizado(ESI_t * pESI)
{
  pthread_mutex_unlock(&m_ESIEjecutandose);
  queue_push(ESIsFinalizados, pESI);
}

void ESIDesconectado( int ESI_ID )
{
  abortESI(ESI_ID);
  printf("El ESI de id %d, fue desconectado\n", ESI_ID);
}

void abortESI( int ESI_ID )
{
  ESI_t * pESI = get_and_remove_ESI_by_ID( ESIsListos->elements, ESI_ID);
  if(pESI) sem_wait(&sem_cantESIsListos);

  if(!pESI)
  {
    pESI = get_and_remove_ESI_by_ID( ESIsBloqueados->elements, ESI_ID);

    if(!pESI)
      pESI = get_and_remove_ESI_by_ID( ESIsFinalizados->elements, ESI_ID);
  }

  printf("El ESI de id %d, fue abortado\n", pESI->id);

  freeESI(pESI);
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
}

void terminarHilo( pthread_t * pHilo )
{
  pthread_join(*pHilo, NULL);
}
