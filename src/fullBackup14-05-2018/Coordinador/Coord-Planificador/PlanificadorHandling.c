/*------------PLANIFICADOR------------*/
#include "PlanificadorHandling.h"

void atenderPlanificador( int socket )
{
  registrarPlanificador( socket );

  fd_set read_fds, master_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&master_fds);

  FD_SET(socket, &master_fds);

  while(1)
  {
    int size;
    void * solicitud;

    read_fds = master_fds;
    select( socket + 1, &read_fds, NULL, NULL, NULL);

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
  socket_planificador = socket;
}

void procesarSolicitudPlanificador(void * solicitud, int size)
{
  puts("Solicitud de Planificador procesada");
}

void planificadorDesconectado()
{
  socket_planificador = -1;
}
