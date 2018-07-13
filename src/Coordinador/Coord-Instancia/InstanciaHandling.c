/*----------INSTANCIA----------*/
#include <string.h>

#include <commons/config.h>

#include "../Coord-Log/coordLog.h"
#include "InstanciaHandling.h"

clave_t * new_clave(inst_t * pInst, char * clave)
{
  clave_t * pClave = malloc(sizeof(clave_t));
  pClave->clave=malloc(strlen(clave)+1);
  strcpy(pClave->clave, clave);
  pClave->instancia = pInst;

  return pClave;
}

inst_t * getInst(char * clave)
{
  clave_t * pClave = get_clave(claves, clave);
  inst_t * pInst;

  if(!pClave)
  {
    log_trace(pLog, "No existe instancia con la clave %s", clave);
    pInst = getInstByEquitativeLoad(clave);
    log_trace(pLog, "Se decidio que la instancia de id %d la contendra", pInst->id);
    list_add(claves, new_clave(pInst, clave));
  }
  else
    pInst = pClave->instancia;

  if(!pInst->connected)
  {
    log_error(pLog, "Error: Instancia desconectada\n");
    return NULL;
  }

  return pInst;
}

inst_t * getInstByEquitativeLoad(char * clave)
{
  static int i;
  i %= coord_Insts->elements_count;

  inst_t* pInst = (inst_t*)list_get(coord_Insts, i++);


  return pInst;
}

void atenderInstancia( int socket )
{
  int * pID, id;

  if( !recvWithBasicProtocol( socket, (void **)&pID ) )
  {
      log_error(pLog, "Instancia de socket %d perdida antes de obtener el ID", socket);
      exit(1);
  }
  id = *pID;
  registrarNuevaInstancia( socket, id);

  tBuffer * pBInfoEntries = newBuffer();
  addIntToBuffer(pBInfoEntries, entryCant);
  addIntToBuffer(pBInfoEntries, entrySize);
  sendWithBasicProtocol(socket, pBInfoEntries->data, pBInfoEntries->size);
  free(pBInfoEntries);

  while(1)
  {
    int size;
    void * solicitud;

    pthread_mutex_lock(&m_ESIAtendido);
    size = recvWithBasicProtocol( socket, &solicitud);
    log_trace(pLog, "Se recibieron datos de la Instancia %d", id);

    if( size ) // SI NO SE DESCONECTO
    {
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
  log_trace(pLog, "Se agrego una nueva Instancia de id = %d a la lista de Instancias", id);
}


inst_t * new_Inst( int id, int socket )
{
  inst_t * pInst = malloc(sizeof(inst_t));
  pInst->id = id;
  pInst->socket = socket;
  pInst->connected = true;
  pInst->claves = list_create();

  return pInst;
}

void procesarSolicitudInstancia(void * solicitud, int size)
{
  if(*((int*)solicitud) == SOLICITUD_ESI_ATENDIENDOSE)
    log_trace(pLog, "Solicitud de ESI atendiendose");
  else
    log_trace(pLog, "Solicitud de Instancia procesada");
}

void instanciaDesconectada( int inst_ID )
{
  //buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
  inst_t * pInst = get_instancia_by_ID( coord_Insts, inst_ID);

  //cambiar el estado de connected a false
  pInst->connected = false;

  log_warning(pLog, "La instancia de id %d, se ha desconectado\n", pInst->id);
}

clave_t * get_clave(t_list * claveList, char * clave)
{
  t_link_element * pAct = claveList->head;
  clave_t * pClave;

  while( pAct != NULL )
  {
    pClave = (clave_t *)(pAct->data);

    if( !strcmp(pClave->clave, clave) )
      return pClave;

    pAct = pAct->next;
  }

  return NULL;
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
