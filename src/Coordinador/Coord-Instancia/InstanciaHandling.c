/*----------INSTANCIA----------*/
#include "InstanciaHandling.h"

inst_t * getInstByEquitativeLoad(char * clave)
{
  static int i;
  i %= coord_Insts->elements_count;

  inst_t* pInst = (inst_t*)list_get(coord_Insts, i++);

  if(!pInst->connected)
    puts("Error: Instancia desconectada, eliminando instancia de lista\n");

  return pInst;
}

void atenderInstancia( int socket )
{
  int * pID, id;

  recvWithBasicProtocol( socket, (void**)&pID);
  id = *pID;
  registrarNuevaInstancia( socket, id);

  while(1)
  {
    int size;
    void * solicitud;

    pthread_mutex_lock(&m_ESIAtendido);
    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
    {
      printf("Instancia id=%d, recibio solicitud\n", id);
      procesarSolicitudInstancia( solicitud, size);
      free(solicitud);
    }
    else
    {
      instanciaDesconectada( id );
      return;
    }
  }

}

void registrarNuevaInstancia( int Inst_socket, int id )
{
  inst_t * pInst = new_Inst( id, Inst_socket);
  list_add( coord_Insts, pInst);
  pInst = (inst_t *)(coord_Insts->head->data);
}


inst_t * new_Inst( int id, int socket )
{
  inst_t * pInst = malloc(sizeof(inst_t));
  pInst->id = id;
  pInst->socket = socket;
  pInst->connected = true;

  return pInst;
}

void procesarSolicitudInstancia(void * solicitud, int size)
{
  if(*((int*)solicitud) == SOLICITUD_ESI_ATENDIENDOSE)
    puts("Solicitud de ESI atendiendose");
  else
    puts("Solicitud de Instancia procesada");
}

void instanciaDesconectada( int inst_ID )
{
  //buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
  inst_t * pInst = get_instancia_by_ID( coord_Insts, inst_ID);

  //cambiar el estado de connected a false
  pInst->connected = false;

  printf("La instancia de id %d, se ha desconectado\n", inst_ID);
}

inst_t * get_instancia_by_ID( t_list * instancias, int id )
{
  t_link_element * pAct = instancias->head;
  inst_t * pInst;

  while( pAct != NULL )
  {
    pInst = (inst_t *)(pAct->data);

    if( is_instancia_ID_equal( pInst, id) )
      return pInst;

    pAct = pAct->next;
  }

  return NULL;
}

bool is_instancia_ID_equal( inst_t * pInst, int id )
{
  return pInst->id == id;
}
