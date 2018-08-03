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
    pInst = getInstByAlg(clave);
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
  i %= coord_Insts.count;

  inst_t* pInst = coord_Insts.insts[i++];


  return pInst;
}

inst_t * getInstByLSU(char * clave)
{
  int i, min = entryCant + 1;
  inst_t * pInst = NULL;
  for(i = 0; i < coord_Insts.count; i++)
    if( coord_Insts.insts[i]->spaceUsed < min )
      pInst = coord_Insts.insts[i];

  return pInst;
}

inst_t * getInstByKE(char * clave)
{
  int deltaAlpha = 'z' - 'a';
  int lettersPerInstance = deltaAlpha/coord_Insts.count;

  int letra = 'a', i;

  for(i = 0; letra <= 'z'; i++)
  {
    int j;
    for( j = 0; j < lettersPerInstance && letra <= 'z'; j++)
    {
      if(clave[0] == letra++)
        return coord_Insts.insts[i];
    }
  }

  return NULL;
}

void atenderInstancia( int socket )
{
  int * pID, id, i;
  char * emptyString = "";
  inst_t * pInst = NULL;

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

  pBInfoEntries = newBuffer();
  for( i = 0; i < coord_Insts.count; i++ )
  {
    if( id == coord_Insts.insts[i]->id )
    {
      pInst = coord_Insts.insts[i];
      pInst->connected=true;
      break;
    }
  }

  if( pInst )
  {
    t_list * claves = pInst->claves;
    t_link_element* pElem = claves->head;

    tBuffer * pBClaves = newBuffer();

    while(pElem)
    {
      clave_t * pClave = pElem->data;
      addStringToBuffer(pBClaves, pClave->clave);
      pElem = pElem->next;
    }

    sendWithBasicProtocol(socket, pBClaves->data, pBClaves->size);

    freeBuffer(pBClaves);
  }
  else
  {
    sendWithBasicProtocol(socket, emptyString, 1);
  }

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

  coord_Insts.insts = realloc(coord_Insts.insts, (++coord_Insts.count)*sizeof(inst_t *));
  coord_Insts.insts[coord_Insts.count-1] = pInst;

  //list_add( coord_Insts, pInst);
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
  inst_t * pInst = get_instancia_by_ID( &coord_Insts, inst_ID);

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

inst_t * get_instancia_by_ID( insts_t * instancias, int id )
{
  int i = 0;
  for(i = 0; i < instancias->count; i++ )
  {
    inst_t * pInst = instancias->insts[i];

    if( is_instancia_ID_equal( pInst, id) )
      return pInst;
  }

  return NULL;
}

bool is_instancia_ID_equal( inst_t * pInst, int id )
{
  return pInst->id == id;
}
