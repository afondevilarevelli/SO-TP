#include <parsi/parser.h>
#include <commons/config.h>

#include "../shared/protocolo.h"
#include "../shared/mySocket.h"

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuertoDePlanificador(t_config * pConf, int * ip, int * puerto);
void obtenerIPyPuerto(t_config * pConf, int * ip, int * puerto, char * ipKey, char * portKey);
int conectarseACoordinador(t_config * pConf, int id);
int conectarseAPlanificador(t_config * pConf, int id);
void enviarIdentificacion(int socket, int id);

FILE * abrirScriptESI(char * ESIScriptFilename);
ESISentenciaParseada_t * newSentencia(int operacion, char * clave, char * valor);
ESISentenciaParseada_t * obtenerSentenciaParseada(FILE * scriptf);
tBuffer * makeBufferFromSentencia(ESISentenciaParseada_t * pSent);
void freeSentencia(ESISentenciaParseada_t * pSent);

orden_t ordenDePlanificador(void);

int socketCoord, socketPlanif;

int main(void)
{
  t_config * pConf = config_create("ESI.config");
  int id = config_get_int_value(pConf, "ESI_ID"), bytes;

  //Primero me conecto al Coordinador
  socketCoord = conectarseACoordinador(pConf, id);

  //Ahora me conecto al Planificador
  socketPlanif = conectarseAPlanificador(pConf, id);

  //Le envio al coordinador una sentencia parseada
  FILE * scriptf = abrirScriptESI("ejemplo.esi");
  ESISentenciaParseada_t * pSent;

  //recibo de orden de ejecucion <--------------- PLANIFICADOR
  while( ordenDePlanificador() == EJECUTAR )
  {
    pSent = obtenerSentenciaParseada(scriptf);

    if(pSent)
    {
      rtdoEjec_t * pRtdo;
      tBuffer * pBuffSent = makeBufferFromSentencia(pSent);
      freeSentencia(pSent);

      //envio de sentenciaParseada -----------> COORDINADOR
      sendWithBasicProtocol(socketCoord, pBuffSent->data, pBuffSent->size);
      freeBuffer(pBuffSent);

      //recibo de resultado de ejecucion <---------- COORDINADOR
      bytes = recvWithBasicProtocol(socketCoord, (void**)&pRtdo);

      //envio de resultado de ejecucion ------------> PLANIFICADOR
      sendWithBasicProtocol(socketPlanif, (void*)pRtdo, bytes);
    }
    else
      break;
  }

  fclose(scriptf);

  rtdoEjec_t finDeEjec = FIN_DE_EJECUCION;
  sendWithBasicProtocol(socketCoord, (void*)&finDeEjec, sizeof(rtdoEjec_t));
  sendWithBasicProtocol(socketPlanif, (void*)&finDeEjec, sizeof(rtdoEjec_t));

  return 0;
}

orden_t ordenDePlanificador(void)
{
  orden_t * pOrden, orden;
  if( !recvWithBasicProtocol(socketPlanif, (void**)&pOrden) )
  {
    puts("No se pudo recibir la orden del planificador porque se desconecto.");
    exit(1);
  }
  orden = *pOrden;
  free(pOrden);

  return orden;
}

/*------SENTENCIAS--------*/

FILE * abrirScriptESI(char * ESIScriptFilename)
{
  FILE * scriptf = fopen(ESIScriptFilename, "r");
  if (scriptf == NULL)
  {
    perror("Error al abrir el archivo: ");
    exit(EXIT_FAILURE);
  }

  return scriptf;
}

ESISentenciaParseada_t * newSentencia(int operacion, char * clave, char * valor)
{
  ESISentenciaParseada_t * pSent = malloc(sizeof(ESISentenciaParseada_t));
  pSent->operacion = operacion;
  pSent->clave = malloc(strlen(clave)+1);
  strcpy(pSent->clave, clave);

  if(valor)
  {
    pSent->valor = malloc(strlen(clave)+1);
    strcpy(pSent->valor, valor);
  }
  else
    pSent->valor = NULL;

  return pSent;
}

ESISentenciaParseada_t * obtenerSentenciaParseada(FILE * scriptf)
{
  ESISentenciaParseada_t * pSent;
  ssize_t read;
  size_t len = 0;
  char * line = NULL;
  if( read = getline(&line, &len, scriptf) != -1)
  {
    t_esi_operacion parsed = parse(line);

    if(parsed.valido)
    {
      switch(parsed.keyword)
      {
          case GET:
              pSent = newSentencia(parsed.keyword, parsed.argumentos.GET.clave, NULL);
              break;
          case SET:
              pSent = newSentencia(parsed.keyword, parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
              break;
          case STORE:
              pSent = newSentencia(parsed.keyword, parsed.argumentos.STORE.clave, NULL);
              break;
          default:
              fprintf(stderr, "No pude interpretar <%s>\n", line);
              exit(EXIT_FAILURE);
      }
    }
    else
    {
      fprintf(stderr, "La linea <%s> no es valida\n", line);
      exit(EXIT_FAILURE);
    }

    destruir_operacion(parsed);

    return pSent;
  }
  else
    return NULL;
}

tBuffer * makeBufferFromSentencia(ESISentenciaParseada_t * pSent)
{
  tBuffer * pBuffer = newBuffer();
  addIntToBuffer(pBuffer, pSent->operacion);
  addStringToBuffer(pBuffer, pSent->clave);
  if(pSent->operacion == SET)
    addStringToBuffer(pBuffer, pSent->valor);

  return pBuffer;
}

void freeSentencia(ESISentenciaParseada_t * pSent)
{
  free(pSent->clave);
  free(pSent->valor);
  free(pSent);
}

/*-------COORDINADOR--------*/

int conectarseACoordinador(t_config * pConf, int id)
{
  int ip, puerto;
  obtenerIPyPuertoDeCoordinador(pConf, &ip, &puerto);
  int socket = connectTo(ip, puerto);

  //Le envio al coordinador que tipo de proceso soy y mi id (en ese orden)
  enviarIdentificacion(socket, id);

  return socket;
}

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto)
{
  obtenerIPyPuerto(pConf, ip, puerto, "COORD_IP", "COORD_PUERTO");
}

/*-------PLANIFICADOR---------*/

int conectarseAPlanificador(t_config * pConf, int id)
{
  int ip, puerto;
  obtenerIPyPuertoDePlanificador(pConf, &ip, &puerto);
  int socket = connectTo(ip, puerto);

  //Le envio al planificador mi id (pues lo unico que acepta son ESIs)
  sendWithBasicProtocol(socket, &id, sizeof(int));

  return socket;
}

void obtenerIPyPuertoDePlanificador(t_config * pConf, int * ip, int * puerto)
{
  obtenerIPyPuerto(pConf, ip, puerto, "PLANIF_IP", "PLANIF_PUERTO");
}

/*--------COMUNES----------*/

void enviarIdentificacion(int socket, int id)
{
  tProceso proceso = ESI;
  sendWithBasicProtocol(socket, &proceso, sizeof(tProceso));
  sendWithBasicProtocol(socket, &id, sizeof(int));
}

void obtenerIPyPuerto(t_config * pConf, int * ip, int * puerto, char * ipKey, char * portKey)
{
  char * strIP = config_get_string_value(pConf, ipKey);
  *ip = inet_addr(strIP);

  //puts(strIP);

  *puerto= htons(config_get_int_value(pConf, portKey));
}
