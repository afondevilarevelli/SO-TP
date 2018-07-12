/*------------PLANIFICADOR------------*/
#include "../Coord-Log/coordLog.h"
#include "PlanificadorHandling.h"

void atenderPlanificador( int socket )
{
  registrarPlanificador( socket );

  struct timeval espera; //0.5 sec


  int max_fd = socket;
  fd_set master_fds, read_fds;
  FD_ZERO(&master_fds);
  FD_SET(socket, &master_fds);

  while(1)
  {
    int size;
    void * solicitud = NULL;
    espera.tv_sec = 0;
    espera.tv_usec = 500000;

    pthread_mutex_lock(&m_planifAviso);/*-------COMIENZO ZONA CRITICA --------*/

    select(max_fd+1, &read_fds, NULL, NULL, &espera);

    if(FD_ISSET(socket, &read_fds))
    {
      size = recvWithBasicProtocol( socket, &solicitud);
      if( size ) // SI NO SE DESCONECTO
        procesarSolicitudPlanificador( solicitud, size);
      else
        planificadorDesconectado();
    }
    pthread_mutex_unlock(&m_planifAviso);/*-------FIN ZONA CRITICA --------*/

    free(solicitud);
    read_fds = master_fds;
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
  log_error(pLog, "Se desconecto el Planificador. El sistema se encuentra en un estado invalido.\nSaliendo del sistema.");
  exit(1);
}
