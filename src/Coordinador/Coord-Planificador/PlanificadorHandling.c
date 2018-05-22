/*------------PLANIFICADOR------------*/
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
}

void procesarSolicitudPlanificador(void * solicitud, int size)
{
  puts("Solicitud de Planificador procesada");
}

void planificadorDesconectado()
{
  socketPlanificador = -1;
}
