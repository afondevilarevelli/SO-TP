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

  sendWithBasicProtocol(socketCoord, &rta, sizeof(rtdoEjec_t));
}

bool puedeEjecutar(int idESI, int op, char * clave)
{
  if(op == 0){ // operacion GET
    cola_clave* c = buscarElementoDeLista(clave);
    if( c->cola == NULL && c->idEsiUsandoClave == (int)NULL){
      c->idEsiUsandoClave = idESI;
      return true;
    }
    else{
      ESI_t* esi = buscarProcesoESI(idESI);
      pthread_mutex_lock(&m_colaListos);
      queue_push(ESIsListos, esi);
      pthread_mutex_unlock(&m_colaListos);
      return false;
    }
  }
  else{
    if(op == 1){ //operacion SET
      cola_clave* c = buscarElementoDeLista(clave);
      int idEsiConClave = c -> idEsiUsandoClave;
      if(idEsiConClave == idESI){
          return true;
      }
      else{
        return false;
      }
    }
    else{ //operacion STORE
      cola_clave* c = buscarElementoDeLista(clave);
      int idEsiConClave = c -> idEsiUsandoClave;
      if(idEsiConClave == idESI){
          pthread_mutex_lock(&m_colaBloqueados);
          c->idEsiUsandoClave = ( (ESI_t*)(queue_pop(c->cola)) )->id;
          pthread_mutex_lock(&m_colaBloqueados);
          return true;
      }
      else{
        return false;
      }
    }
  }
}

void coordinadorDesconectado()
{
  socketCoord = -1;
  log_error(pLog, "Se desconecto el Coordinador. El sistema se encuentra en un estado invalido.\nSaliendo del sistema.");
  exit(1);
}
