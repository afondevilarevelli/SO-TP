/*------------ESI-----------*/
#include "../Coord-Log/coordLog.h"
#include "ESIHandling.h"
#include "../Coord-Instancia/InstanciaHandling.h"

void ESIFinalizado(int ESI_ID)
{
  //ESIDesconectado(ESI_ID);
}

void atenderESI( int socket )
{
  int * pID, id;
  ESI_t * pESI;

  if( !recvWithBasicProtocol( socket, (void **)&pID ) )
  {
      log_error(pLog, "ESI de socket %d perdido antes de obtener el ID", socket);
      exit(1);
  }
  id = *pID;

  if( !(pESI = get_ESI_by_ID( coord_ESIs, id)) )
    registrarNuevoESI( socket, id);
  else
  {
    log_error(pLog, "El ESI de id %d ya existe en el sistema", id);
    return;
  }

  int size;
  do
  {
    void * solicitud;

    //recibo solicitud <----------------- ESI
    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
    {
      log_trace(pLog, "Se recibieron datos del ESI %d", id);
      switch ( *((rtdoEjec_t*)solicitud) )
      {
        case FIN_DE_EJECUCION:
          log_trace(pLog, "El ESI con ID  =%d ha finalizado con éxito", id);
          break;

        case ABORTED:
          log_error(pLog, "El ESI con ID = %d ha sido abortado", id);
          break;

        case SENTENCIA:
        {
          rtdoEjec_t rtdo;
          if( coord_Insts->elements_count )
          {
            rtdo = procesarSolicitudESI(solicitud + sizeof(rtdoEjec_t), size);
            log_debug(pLog, "La solicitud del ESI con ID = %d fue %s", id, rtdo==SUCCESS?"procesada con exito":"un fracaso");
          }
          else
          {
            rtdo = NO_HAY_INSTANCIAS_CONECTADAS;
            log_warning(pLog, "No hay instancias conectas");
          }
          free(solicitud);

          //informo de resultado de ejecucion -------------> ESI
          sendWithBasicProtocol(socket, (void**)&rtdo, sizeof(rtdoEjec_t));
          log_trace(pLog, "Se informa de ese resultado al ESI con ID = %d", id);
        }
          break;

        default:
          log_error(pLog, "Solicitud desconocida");
      }//endswitch
    }//end if(size)
  }
  while(size);

  ESIDesconectado(id);

}

void registrarNuevoESI( int ESI_socket, int id )
{
  ESI_t * pESI = new_ESI( id, ESI_socket);
  list_add( coord_ESIs, pESI);
  log_trace(pLog, "Se agrego un nuevo ESI con ID = %d a la lista de ESIs", id);
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
  log_info(pLog,"Sentencia ESI: op=%s, clave=%s, valor=%s\n", sent.operacion==GET?"GET":sent.operacion==SET?"SET":"STORE", sent.clave, sent.valor?sent.valor:"No corresponde");

  inst_t * pInst = getInstByEquitativeLoad(sent.clave);
  log_trace(pLog,"Se obtuvo la instancia de id = %d por Equitative Load", pInst->id);

  sendWithBasicProtocol( pInst->socket, solicitud, size);
  log_trace(pLog, "Se le envio la sentencia a la instancia %d", pInst->id);

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
  log_trace(pLog, "Se recibio el resultado de la ejecucion de la sentencia en la instancia %d", pInst->id);

  pthread_mutex_unlock(&m_ESIAtendido);
  //retornar rtdoEjec
  return *rtdo;
}

void ESIDesconectado( int ESI_ID )
{
  //buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
  ESI_t * pESI = get_and_remove_ESI_by_ID( coord_ESIs, ESI_ID);

  //cambiar el estado de connected a false
  pESI->connected = false;

  log_warning(pLog, "El ESI con ID = %d, se ha desconectado y fue eliminado de los ESIs a coordinar.\n", pESI->id);
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
