#ifndef ESI_HANDLING_H
#define ESI_HANDLING_H

#include <stdio.h>
#include <pthread.h>
#include <commons/collections/list.h>

#include "../../shared/testConnection.h"
#include "../../shared/mySocket.h"
#include "../../shared/protocolo.h"

typedef struct
    {
      int id;
      int socket;
      bool connected;
    } ESI_t;
extern t_list * coord_ESIs;

extern pthread_mutex_t m_ESIAtendido;

void atenderESI(int socket);
void registrarNuevoESI( int ESI_socket, int id );
ESI_t * new_ESI( int id, int socket );
void ESIDesconectado( int ESI_ID );
void ESIFinalizado(int ESI_ID);
ESI_t * get_ESI_by_ID( t_list * ESIs, int id );
ESI_t * get_and_remove_ESI_by_ID( t_list * ESIs, int id );
bool is_ESI_ID_equal( ESI_t * pESI, int id );

rtdoEjec_t procesarSolicitudESI(int id, void * solicitud, int size);

#endif
