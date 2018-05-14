/*------------ESI-----------*/
#include "ESiHandling.h"

void atenderESI( int socket )
{
  int * pID, id;

  recvWithBasicProtocol( socket, (void **)&pID);
  id = *pID;
  registrarNuevoESI( socket, id);

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
    {
      procesarSolicitudESI( solicitud, size);
      free(solicitud);
    }
    else
    {
      ESIDesconectado( id );
      return;
    }

  }

}

void registrarNuevoESI( int ESI_socket, int id )
{
  ESI_t * pESI = new_ESI( id, ESI_socket);
  list_add( coord_ESIs, pESI);
}

ESI_t * new_ESI( int id, int socket )
{
  ESI_t * pESI = malloc(sizeof(ESI_t));
  pESI->id = id;
  pESI->socket = socket;
  pESI->connected = true;

  return pESI;
}

void procesarSolicitudESI(void * solicitud, int size)
{
  puts("Solicitud ESI procesada");
}

void ESIDesconectado( int ESI_ID )
{
  //buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
  ESI_t * pESI = get_ESI_by_ID( coord_ESIs, ESI_ID);

  //cambiar el estado de connected a false
  pESI->connected = false;

  printf("El ESI de id %d, se ha desconectado\n", ESI_ID);
}

ESI_t * get_ESI_by_ID( t_list * ESIs, int id )
{
  t_link_element * pAct = ESIs->head;
  ESI_t * pESI;

  while( pAct != NULL )
  {
    pESI = (ESI_t *)(pAct->data);

    if( is_ESI_ID_equal( pESI, id) )
      return pESI;

    pAct = pAct->next;
  }

  return NULL;
}

bool is_ESI_ID_equal( ESI_t * pESI, int id )
{
  return pESI->id == id;
}
