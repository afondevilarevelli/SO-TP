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
{GET, "beta", NULL},
{SET, "ceta", "hongo"},
{GET, "deta", NULL}

};
  //obtiene datos de inicializacion
  int ip, puerto, coordSock, bytes, i, id;
  tProceso tProc = ESI;
  rtdoEjec_t rtdo;
  char * strIP;
  void * msg;
  tBuffer * pBuff;

  pConf = config_create("esiDummy.config");

  strIP = config_get_string_value(pConf,"COORD_IP");
  ip = getIntegerIP(strIP);
  free(strIP);

  puerto = config_get_int_value(pConf, "COORD_PUERTO");

  //se conecta al coordinador
  coordSock = connectTo(ip, htons(puerto));

  //le dice que es de tipo ESI
  sendWithBasicProtocol(coordSock, &tProc, sizeof(tProceso));

  //le envia su ID
  id = config_get_int_value(pConf, "ESI_ID");
  sendWithBasicProtocol(coordSock, &id, sizeof(int));

  int cantSent = sizeof(sentencias)/sizeof(t_sent);
  for( i = 0; i < cantSent; i++)
  {
    //le envia una sentencia
    pBuff = newBuffer();
    addIntToBuffer(pBuff, SENTENCIA);
    addIntToBuffer(pBuff, sentencias[i].op);
    addStringToBuffer(pBuff, sentencias[i].clave);
    if(sentencias[i].valor)
      addStringToBuffer(pBuff, sentencias[i].valor);

    sendWithBasicProtocol(coordSock, pBuff->data, pBuff->size);

    freeBuffer(pBuff);

    //espera un rtdoEjec_t
    bytes = recvWithBasicProtocol(coordSock, &msg);
    pBuff = makeBuffer(msg, bytes);

    rtdo = readIntFromBuffer(pBuff);
    switch( rtdo )
    {
      case SUCCESS:
        puts("SUCCESS");
        break;

      case FAILURE:
        puts("FAILURE");
        break;

      case NO_HAY_INSTANCIAS_CONECTADAS:
        puts("NO HAY INSTANCIAS CONECTADAS");
        break;

      default:
        printf("No se comprende el mensaje del coordinador(%d)\n", bytes);
    }

    freeBuffer(pBuff);
  }

  close(coordSock);
  config_destroy(pConf);

  return 0;
}
