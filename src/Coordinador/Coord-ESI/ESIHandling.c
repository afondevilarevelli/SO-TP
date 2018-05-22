/*------------ESI-----------*/
#include "ESIHandling.h"
#include "../Coord-Instancia/InstanciaHandling.h"

void ESIFinalizado(int ESI_ID)
{
  ESIDesconectado(ESI_ID);
}

void atenderESI( int socket )
{
  int * pID, id;

  if( !recvWithBasicProtocol( socket, (void **)&pID ) )
  {
      puts("ESI perdido antes de obtener el ID");
      exit(1);
  }
  id = *pID;
  registrarNuevoESI( socket, id);

  while(1)
  {
    int size;
    void * solicitud;

    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
    {
      if( ( size == sizeof(int) ) && ((*((int*)solicitud)) == FIN_DE_EJECUCION) )
      {
          break;
      }
      else
      {
        rtdoEjec_t rtdo = procesarSolicitudESI(solicitud, size);
        puts(rtdo==SUCCESS?"Es success posta":"No es success");
        sendWithBasicProtocol(socket, (void**)&rtdo, sizeof(rtdoEjec_t));
        free(solicitud);
      }
    }
    else
    {
      ESIDesconectado( id );
      return;
    }

  }

  ESIFinalizado(id);

}

void registrarNuevoESI( int ESI_socket, int id )
{
  ESI_t * pESI = new_ESI( id, ESI_socket);
  list_add( coord_ESIs, pESI);
}

ESI_t * new_ESI( int id, int socket )
{
  ESI_t * pESI = malloc(sizeof(ESI_t));
  pESI->id = id;
  pESI->socket = socket;
  pESI->connected = true;

  return pESI;
}

rtdoEjec_t procesarSolicitudESI(void * solicitud, int size)
{
  tBuffer * buffSent = makeBuffer(solicitud, size);
  ESISentenciaParseada_t sent;
  sent.operacion = readIntFromBuffer(buffSent);
  sent.clave = readStringFromBuffer(buffSent);
  if(sent.operacion == SET)
    sent.valor = readStringFromBuffer(buffSent);
  else
    sent.valor = NULL;

  freeBuffer(buffSent);

  printf("op=%d, clave=%s, valor=%s\n", sent.operacion, sent.clave, sent.valor?sent.valor:"No corresponde");
  inst_t * pInst = getInstByEquitativeLoad(sent.clave);
  printf("size=%d\n", size);
  sendWithBasicProtocol( pInst->socket, solicitud, size);

  //si es un GET
    //preguntarle al planificador si la clave esta disponible
  //si es un STORE o un SET
    //obtener instancia que tiene la clave
    //si es un STORE
      //guardar el valor actual de la tabla de entradas de la instancia asociado a la clave en el disco duro
      //avisarle al planificador que se libero la clave (no chequea que el STORE lo haga el mismo ESI que hizo el GET)
    //si es un SET
      //cambiar el valor actual de la tabla de entradas de la instancia asociado a la clave por el nuevo
  rtdoEjec_t * rtdo;
  recvWithBasicProtocol(pInst->socket, (void**)&rtdo);
  pthread_mutex_unlock(&m_ESIAtendido);
  puts("Solicitud ESI procesada");
  //retornar rtdoEjec
  return *rtdo;
}

void ESIDesconectado( int ESI_ID )
{
  //buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
  ESI_t * pESI = get_ESI_by_ID( coord_ESIs, ESI_ID);

  //cambiar el estado de connected a false
  pESI->connected = false;

  printf("El ESI de id %d, se ha desconectado\n", pESI->id);
}

ESI_t * get_ESI_by_ID( t_list * ESIs, int id )
{
  t_link_element * pAct = ESIs->head;
  ESI_t * pESI;

  while( pAct != NULL )
  {
    pESI = (ESI_t *)(pAct->data);

    if( is_ESI_ID_equal( pESI, id) )
      return pESI;

    pAct = pAct->next;
  }

  return NULL;
}

bool is_ESI_ID_equal( ESI_t * pESI, int id )
{
  return pESI->id == id;
}
