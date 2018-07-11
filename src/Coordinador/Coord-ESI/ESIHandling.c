/*------------ESI-----------*/
#include "../Coord-Log/coordLog.h"
#include "ESIHandling.h"
#include "../Coord-Instancia/InstanciaHandling.h"
#include "../Coord-Planificador/PlanificadorHandling.h"

void ESIFinalizado(int ESI_ID)
{
  //ESIDesconectado(ESI_ID);
}

void atenderESI( int socket )
{
  int * pID, id;

  if( !recvWithBasicProtocol( socket, (void **)&pID ) )
  {
      log_error(pLog, "ESI de socket %d perdido antes de obtener el ID", socket);
      exit(1);
  }
  id = *pID;

  if( !get_ESI_by_ID( coord_ESIs, id) )
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
            rtdo = procesarSolicitudESI(id, solicitud + sizeof(rtdoEjec_t), size);
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

rtdoEjec_t procesarSolicitudESI(int id, void * solicitud, int size)
{
  tBuffer *buffSent, *buffAviso;
  ESISentenciaParseada_t sent;

  buffSent = makeBuffer(solicitud, size);
  buffAviso = newBuffer();

  sent.operacion = readIntFromBuffer(buffSent);
  sent.clave = readStringFromBuffer(buffSent);
  if(sent.operacion == SET)
    sent.valor = readStringFromBuffer(buffSent);
  else
    sent.valor = NULL;

  freeBuffer(buffSent);
  log_info(pLog,"Sentencia ESI: op=%s, clave=%s, valor=%s\n", sent.operacion==GET?"GET":sent.operacion==SET?"SET":"STORE", sent.clave, sent.valor?sent.valor:"No corresponde");

  addIntToBuffer(buffAviso, id);
  addIntToBuffer(buffAviso, sent.operacion);
  addStringToBuffer(buffAviso, sent.clave);

  pthread_mutex_lock(&m_planifAviso);/*-------COMIENZO ZONA CRITICA --------*/

  sendWithBasicProtocol(socketPlanificador, buffAviso->data, buffAviso->size);
  log_trace(pLog, "Se informa al Planificador de la operacion y se espera su respuesta");

  rtdoEjec_t *rtaPlanif, *rtdo;
  int bytes = recvWithBasicProtocol(socketPlanificador, (void**)&rtaPlanif);

  pthread_mutex_unlock(&m_planifAviso);/*-------FIN ZONA CRITICA --------*/
  if(bytes)
  {
    switch(*rtaPlanif)
    {
      case SUCCESS:
        log_trace(pLog, "El Planificador aprobo la operacion");

        inst_t * pInst = getInstByEquitativeLoad(sent.clave);
        log_trace(pLog, "Se obtuvo la instancia de id = %d por Equitative Load", pInst->id);

        sendWithBasicProtocol( pInst->socket, solicitud, size);
        log_trace(pLog, "Se le envio la sentencia a la instancia %d", pInst->id);

        recvWithBasicProtocol(pInst->socket, (void**)&rtdo);
        log_trace(pLog, "Se recibio el resultado de ejecucion de la sentencia en la instancia %d", pInst->id);

        break;
      case FAILURE:
        log_trace(pLog, "El Planificador rechazo la operacion");
        *rtdo = FAILURE;

        break;
      default:
        log_error(pLog, "No se entiende la respuesta del Planificador");
        *rtdo = FAILURE;
    }
  }
  else // !bytes
  {
    planificadorDesconectado();
  }

  pthread_mutex_unlock(&m_ESIAtendido);

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

      ESIs->elements_count--;
      return pESI;
    }
    pPrev = pAct;
    pAct = pAct->next;
  }

  return NULL;
}
