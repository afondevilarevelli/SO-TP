/*------------PLANIFICADOR------------*/
#include "../Coord-Log/coordLog.h"
#include "PlanificadorHandling.h"

void atenderPlanificador( int socket )
{
  registrarPlanificador( socket );

  while(1)
  {
    int size;
    void * solicitud = NULL;

    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
      procesarSolicitudPlanificador( solicitud, size);
    else
      planificadorDesconectado();

    free(solicitud);
  }

}

void registrarPlanificador( int socket )
{
  socketPlanificador = socket;
  log_trace(pLog, "Se conecto el Planificador (socket %d)", socketPlanificador);
}

void procesarSolicitudPlanificador(void * solicitud, int size)
{
  log_trace(pLog, "Solicitud de Planificador procesada");
}

void planificadorDesconectado()
{
  socketPlanificador = -1;
  log_error(pLog, "Se desconecto el Planificador. El sistema se encuentra en un estado invalido.");
}
