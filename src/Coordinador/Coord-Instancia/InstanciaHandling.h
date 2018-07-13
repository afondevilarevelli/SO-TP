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
    int spaceUsed;
    bool connected;
    t_list * claves;
  } inst_t;


typedef struct
  {
    inst_t ** insts;
    int count;
  } insts_t;

extern insts_t coord_Insts;

typedef struct
  {
      char * clave;
      inst_t * instancia;
  } clave_t;

extern t_list * claves;
//extern t_list * coord_Insts;

extern pthread_mutex_t m_ESIAtendido;
extern int entrySize, entryCant;

typedef inst_t *(*fgetInstAlg)(char*);

extern fgetInstAlg getInstByAlg;

void atenderInstancia(int socket);
void registrarNuevaInstancia( int inst_socket, int id );
inst_t * new_Inst( int id, int socket );
void instanciaDesconectada(int inst_ID);

clave_t * new_clave(inst_t * pInst, char * clave);
inst_t * getInst(char * clave);
clave_t * get_clave(t_list * claveList, char * clave);
inst_t * get_instancia_by_ID( insts_t * instancias, int id );
bool is_instancia_ID_equal( inst_t * pInst, int id );

void procesarSolicitudInstancia(void * solicitud, int size);

inst_t * getInstByEquitativeLoad(char * clave);
inst_t * getInstByLSU(char * clave);
inst_t * getInstByKE(char * clave);

#endif
