#include "../../shared/mySocket.h"
#include "../../shared/buffer.h"
#include "../../shared/protocolo.h"
#include <commons/config.h>

typedef struct
  {
    int op;
    char * clave;
    char * valor;
  } t_sent;

t_config * pConf;

int main(void)
{
  t_sent sentencias[] =
{

{SET, "alfa", "toro"},
{SET, "alfa", "to"},
{GET, "beta", NULL},
{SET, "ceta", "hongo"},
{STORE, "alfa", NULL},
{SET, "re", "ooo"},
{SET, "df", "hoo"},
{SET, "cetiia", "oo"},
{SET, "ceuyta", "hooo"},
{SET, "jh", "hongoooooo"}
//{GET, "deta", NULL}

};
  //obtiene datos de inicializacion
  int ip, puerto, listener, instSock, bytes, i;
  char * strIP;
  void * msg;
  pConf = config_create("coordDummy.config");

  strIP = config_get_string_value(pConf,"COORD_IP");
  ip = getIntegerIP(strIP);
  free(strIP);

  puerto = config_get_int_value(pConf, "COORD_PUERTO");

  //espera conexion de instancia
  listener = listenOn(ip, htons(puerto));
  instSock = acceptClient(listener);

  //recibe el tipo INSTANCIA
  recvWithBasicProtocol(instSock, &msg);
  free(msg);

  //recibe el ID de la instancia
  recvWithBasicProtocol(instSock, &msg);
  free(msg);

  //le envia cantidad y tamaño de entradas
  tBuffer * pBuff = newBuffer();
  addIntToBuffer(pBuff, 4);
  addIntToBuffer(pBuff, 7);

  sendWithBasicProtocol(instSock, pBuff->data, pBuff->size);

  freeBuffer(pBuff);

  int cantSent = sizeof(sentencias)/sizeof(t_sent);
  for( i = 0; i < cantSent; i++)
  {
    //le envia una orden de acceso dummy
    pBuff = newBuffer();
    addIntToBuffer(pBuff, sentencias[i].op);
    addStringToBuffer(pBuff, sentencias[i].clave);
    if(sentencias[i].valor)
      addStringToBuffer(pBuff, sentencias[i].valor);

    sendWithBasicProtocol(instSock, pBuff->data, pBuff->size);

    freeBuffer(pBuff);

    //espera un SOLICITUD_ESI_ATENDIENDOSE
    bytes = recvWithBasicProtocol(instSock, &msg);
    pBuff = makeBuffer(msg, bytes);

    bytes = readIntFromBuffer(pBuff);
    switch( bytes )
    {
      case SOLICITUD_ESI_ATENDIENDOSE:
        puts("La instancia indica que esta en ello");
        break;

        default:
        printf("No se comprende el mensaje de la instancia (%d)\n", bytes);
    }

    freeBuffer(pBuff);

    //espera un rtdoEjec_t
    bytes = recvWithBasicProtocol(instSock, &msg);
    pBuff = makeBuffer(msg, bytes);

    switch( readIntFromBuffer(pBuff) )
    {
      case SUCCESS:
        puts("Todo bien");
        break;

      default:
        puts("No fue SUCCESS");
    }

    freeBuffer(pBuff);
  }

  close(instSock);
  config_destroy(pConf);

  return 0;
}
