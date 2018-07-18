/*------------PLANIFICADOR------------*/
#include "CoordHandling.h"

void atenderCoordinador( int socket )
{
  while(1)
  {
    int size;
    void * solicitud = NULL;

    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
      procesarSolicitudCoordinador( solicitud, size);
    else
      coordinadorDesconectado();

    free(solicitud);
  }
}

void procesarSolicitudCoordinador(void * solicitud, int size)
{
  tBuffer * buffAviso = makeBuffer(solicitud, size);
  int idESI = readIntFromBuffer(buffAviso);
  int operacion = readIntFromBuffer(buffAviso);
  char * clave = readStringFromBuffer(buffAviso);

  log_trace(pLog, "El Coordinador pregunta si el ESI %d puede ejecutar\n"
                    "\tLa operacion %s sobre la clave %s\n",
                    idESI, operacion==0?"GET":operacion==1?"SET":operacion==2?"STORE":"???", clave);

  rtdoEjec_t rta = FAILURE;

  if( puedeEjecutar(idESI, operacion, clave) )
    rta = SUCCESS;
  else if( !buscarElementoDeLista(clave) )
    rta = ABORTED;

  sendWithBasicProtocol(socketCoord, &rta, sizeof(rtdoEjec_t));
}

bool puedeEjecutar(int idESI, int op, char * clave)
{
  if(op == GET)
  { // operacion GET
    cola_clave* c = buscarElementoDeLista(clave);
    if( !c )
    {
      list_add(ListaColas, new_cola_clave(clave, idESI));
      return true;
    }
    else
    {
      ESI_t* esi = buscarProcesoESI(idESI);
      esi->state = BLOQUEADO;
      queue_push(c->cola,esi);
      pthread_mutex_lock(&m_colaBloqueados);
      queue_push(ESIsBloqueados, esi);
      pthread_mutex_unlock(&m_colaBloqueados);
      return false;
    }
  }
  else if(op == SET)
  { //operacion SET
      cola_clave* c = buscarElementoDeLista(clave);
      if(c){ 
        int idEsiConClave = c -> idEsiUsandoClave;
        return idEsiConClave == idESI;
      }
      else{
        return false;
      }
  }
  else
  { //operacion STORE
    cola_clave* c = buscarElementoDeLista(clave);
    if(c)
    {
      int idEsiConClave = c -> idEsiUsandoClave;
      if(idEsiConClave == idESI)
      {
        if(!queue_is_empty(c->cola)){ 
        ESI_t* elESI = (ESI_t*)(queue_pop(c->cola));
        elESI->state = NORMAL;
        c->idEsiUsandoClave = elESI->id;
        return true;
        }
        else{
          c->idEsiUsandoClave = 0;
        }
      }
      else
      {
        return false;
      }
    }
    else 
    {
      return false;
    }
  }
}
void coordinadorDesconectado()
{
  socketCoord = -1;
  log_error(pLog, "Se desconecto el Coordinador. El sistema se encuentra en un estado invalido.\nSaliendo del sistema.");
  exit(1);
}
