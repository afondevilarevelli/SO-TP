#include "../../shared/mySocket.h"
#include "../../shared/protocolo.h"

#include <commons/config.h>

t_config * pConf;

int main(void)
{
  int ip, puerto, coordSock, bytes;
  int tipoPr = INSTANCIA, id, rtdo = SUCCESS;
  char * strIP;
  void * msg;
  pConf = config_create("instDummy.config");

  strIP = config_get_string_value(pConf, "COORD_IP");
  ip = getIntegerIP(strIP);
  free(strIP);

  puerto = config_get_int_value(pConf, "COORD_PUERTO");

  id = config_get_int_value(pConf, "INST_ID");

  //se conecta al Coordinador
  coordSock = connectTo(ip, htons(puerto));

  //le dice que es INSTANCIA
  sendWithBasicProtocol(coordSock, &tipoPr, sizeof(int));

  //le envia su id
  sendWithBasicProtocol(coordSock, &id, sizeof(int));

  //recibe cant y tamanio de entradas
  bytes = recvWithBasicProtocol(coordSock, &msg);
  free(msg);

  while(bytes)
  {
    //recibe solicitud de acceso a recurso
    bytes = recvWithBasicProtocol(coordSock, &msg);
    free(msg);

    //le envia el resultado SUCCESS o FAILURE
    sendWithBasicProtocol(coordSock, &rtdo, sizeof(int));
  }

  config_destroy(pConf);

  return 0;
}
