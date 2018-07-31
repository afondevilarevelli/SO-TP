#include "../../shared/mySocket.h"
#include "../../shared/protocolo.h"

#include <commons/config.h>

t_config * pConf;

int main(void)
{
  int ip, puerto, coordSock, i;
  tProceso tProc = PLANIFICADOR;
  rtdoEjec_t rtdo[] =
{
  SUCCESS,
  FAILURE,
  SUCCESS,
  SUCCESS,
  FAILURE,
  SUCCESS
};
  char * strIP;
  void * msg;
  int cantElem = sizeof(rtdo)/sizeof(rtdoEjec_t);

  pConf = config_create("planifDummy.config");

  strIP = config_get_string_value(pConf, "COORD_IP");
  ip = getIntegerIP(strIP);
  free(strIP);

  puerto = config_get_int_value(pConf, "COORD_PUERTO");

  //se conecta al Coordinador
  coordSock = connectTo(ip, htons(puerto));

  //le dice que es PLANIFICADOR
  sendWithBasicProtocol(coordSock, &tProc, sizeof(tProceso));

  //mientras siga prendido
  for(i = 0; i < cantElem; i++)
  {
    //espera solicitud del coordinador:
    recvWithBasicProtocol(coordSock, &msg);
      //idDeESI
      //operacion
      //clave
    free(msg);

    sendWithBasicProtocol(coordSock, rtdo + i, sizeof(rtdoEjec_t));
    //responde SUCCESS o FAILURE
  }

  config_destroy(pConf);

  return 0;
}
