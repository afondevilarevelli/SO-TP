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
  return true;
}

void coordinadorDesconectado()
{
  socketCoord = -1;
  log_error(pLog, "Se desconecto el Coordinador. El sistema se encuentra en un estado invalido.\nSaliendo del sistema.");
  exit(1);
}
