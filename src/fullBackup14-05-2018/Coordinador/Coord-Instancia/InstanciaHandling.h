#ifndef INSTANCIA_HANDLING_H
#define INSTANCIA_HANDLING_H

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
    } inst_t;
t_list * coord_Insts;

void atenderInstancia(int socket);
void registrarNuevaInstancia( int inst_socket, int id );
inst_t * new_Inst( int id, int socket );
void instanciaDesconectada(int inst_ID);
inst_t * get_instancia_by_ID( t_list * instancias, int id );
bool is_instancia_ID_equal( inst_t * pInst, int id );

void procesarSolicitudInstancia(void * solicitud, int size);

#endif
