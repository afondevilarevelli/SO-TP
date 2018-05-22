#ifndef PLANIFICADOR_INSTANCIA_H
#define PLANIFICADOR_INSTANCIA_H

#include <stdio.h>
#include <pthread.h>
#include <commons/collections/list.h>

#include "../../shared/testConnection.h"
#include "../../shared/mySocket.h"
#include "../../shared/protocolo.h"


int socket_planificador;

void atenderPlanificador(int socket);
void registrarPlanificador(int socket);
void planificadorDesconectado(void);
void procesarSolicitudPlanificador(void * solicitud, int size);

#endif
