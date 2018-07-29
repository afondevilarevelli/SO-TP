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

{SET, "nintendo:consola:nes", "mario"},
{SET, "nintendo:consola:snes", "SuperMario"},
{SET, "nintendo:consola:nintendo64", "Mario64"},
//{STORE, "alfa", NULL},
{SET, "nintendo:consola:gamecube", "FinalFantasyTactics"},
{SET, "nintendo:consola:wii", "MarioGalaxy"},
{SET, "nintendo:consola:wii", "MarioGalaxy2"},
{SET, "nintendo:consola:wiiu", "RIP"},
{SET, "nintendo:consola:nintendo64", "MarioKart"},
{SET, "sony:consola:ps1", "FF7"},
{SET, "nintendo:consola:nintendo64", "PkmnSnap"},
{SET, "sony:consola:ps2", "GodOfWar"},
{SET, "sony:consola:ps1", "FF8"},
//{SET, "sony:consola:ps3", "TLOU"},
//{SET, "sony:consola:ps2", "GodOfWar2"},
//{SET, "sony:consola:ps4", "Bloodborne"},
//{SET, "sony:consola:ps3", "GodOfWar3"},
//{SET, "microsoft:consola:xbox", "Halo"},
//{SET, "microsoft:consola:xbox360", "Halo"},
//{SET, "microsoft:consola:xboxone", "Halo"},
//{SET, "h", "hooo"},
//{SET, "h", "hooo"},
{STORE, "sony:consola:ps1", NULL},
{SET, "nintendo:consola:switch", "ZeldaBOTW"}

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
  addIntToBuffer(pBuff, 8);
  addIntToBuffer(pBuff, 10);

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
