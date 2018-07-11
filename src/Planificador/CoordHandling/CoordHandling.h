#ifndef COORD_HANDLING_H
#define COORD_HANDLING_H

#include <stdio.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/log.h>

#include "../../shared/testConnection.h"
#include "../../shared/mySocket.h"
#include "../../shared/protocolo.h"
#include "../../shared/buffer.h"


extern int socketCoord;
extern t_log * pLog;

void atenderCoordinador(int socket);
void coordinadorDesconectado(void);
void procesarSolicitudCoordinador(void * solicitud, int size);
bool puedeEjecutar(int idESI, int op, char * clave);

#endif