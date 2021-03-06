#include "parsi/src/parsi/parser.h"
#include <errno.h>

#include <commons/config.h>
#include <commons/log.h>

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
void addSentenciaToBuffer(tBuffer * pBuffer, ESISentenciaParseada_t * pSent);
tBuffer * makeBufferFromSentencia(ESISentenciaParseada_t * pSent);
void freeSentencia(ESISentenciaParseada_t * pSent);

orden_t ordenDePlanificador(void);

int socketCoord, socketPlanif;

t_log * pLog;

int main(void)
{
  pLog = log_create("ESI.log", "ESI", true, LOG_LEVEL_TRACE);
  log_trace(pLog, "Iniciando...");

  t_config * pConf = config_create("ESI.config");
  int id = config_get_int_value(pConf, "ESI_ID"), bytes;
  log_trace(pLog, "Se obtiene ID %d del archivo de configuracion", id);

  //Primero me conecto al Coordinador
  socketCoord = conectarseACoordinador(pConf, id);
  log_trace(pLog, "El esi de id = %d se conecto al Coordinador en el socket %d",id, socketCoord);

  //Ahora me conecto al Planificador
  socketPlanif = conectarseAPlanificador(pConf, id);
  log_trace(pLog, "El esi de id = %d se conecto al Planificador en el socket %d",id, socketPlanif);

  //Le envio al coordinador una sentencia parseada
  FILE * scriptf = abrirScriptESI(config_get_string_value(pConf, "ESI_SCRIPT"));
  log_trace(pLog, "Script ejemplo.esi abierto");

  ESISentenciaParseada_t * pSent;
  orden_t orden;

  //recibo de orden de ejecucion <--------------- PLANIFICADOR
  log_trace(pLog, "El esi de id = %d esta esperando orden de Planificador");
  while( (orden = ordenDePlanificador()) == EJECUTAR )
  {
    int prevPos = ftell(scriptf);
    pSent = obtenerSentenciaParseada(scriptf);
    if(pSent)
    {
      log_debug(pLog, "La sentencia parseada del esi con id = %d es:\n"
                      "Op = %s\n"
                      "Clave = %s\n"
                      "Valor = %s\n",
                      id,
                      pSent->operacion==GET?"GET":pSent->operacion==SET?"SET":"STORE", pSent->clave, pSent->valor?pSent->valor:"No corresponde");

      rtdoEjec_t * pRtdo;
      tBuffer * pBuffSent = newBuffer();
      addIntToBuffer(pBuffSent, SENTENCIA);
      addSentenciaToBuffer(pBuffSent, pSent);
      freeSentencia(pSent);

      //envio de sentenciaParseada -----------> COORDINADOR
      sendWithBasicProtocol(socketCoord, pBuffSent->data, pBuffSent->size);
      log_trace(pLog, "El esi de id = %d ha enviado la sentencia al Coordinador", id);
      freeBuffer(pBuffSent);

      //recibo de resultado de ejecucion <---------- COORDINADOR
      log_trace(pLog, "El esi de id = %d espera el resultado de ejecucion de Coordinador", id);
      bytes = recvWithBasicProtocol(socketCoord, (void**)&pRtdo);
      log_debug(pLog, "El esi de id = %d recibe el resultado de %s",id, *pRtdo==SUCCESS?"SUCCESS":*pRtdo==FAILURE?"FAILURE":"ERROR");
     // if(*pRtdo == FAILURE)
       // fseek(scriptf, prevPos, SEEK_SET);

      //envio de resultado de ejecucion ------------> PLANIFICADOR
      sendWithBasicProtocol(socketPlanif, (void*)pRtdo, bytes);
      log_trace(pLog, "El esi de id = %d ha enviado el resultado al Planificador",id);
    }
    else
      break;

    log_trace(pLog, "El esi de id = %d espera orden de Planificador", id);
  }

  fclose(scriptf);

  if( orden == ABORTAR )
  {
    rtdoEjec_t aborted = ABORTED;
    log_error(pLog, "El ESI de id = %d fue abortado", id);
    sendWithBasicProtocol(socketCoord, (void*)&aborted, sizeof(rtdoEjec_t));
    log_trace(pLog, "Se informa del aborto del esi de id = %d al Coordinador", id);
  }
  else if(!pSent)
  {
    rtdoEjec_t finDeEjec = FIN_DE_EJECUCION;
    log_trace(pLog, "El ESI de id = %d finalizo su ejecucion correctamente", id );

    sendWithBasicProtocol(socketCoord, (void*)&finDeEjec, sizeof(rtdoEjec_t));
    log_trace(pLog, "Se informa del fin de ejecucion del esi de id = %d al Coordinador", id);

    sendWithBasicProtocol(socketPlanif, (void*)&finDeEjec, sizeof(rtdoEjec_t));
    log_trace(pLog, "Se informa del fin de ejecucion del esi de id = %d al PLanificador", id );
  }
  else
  {
    log_error(pLog, "ORDEN DESCONOCIDA DEL PLANIFICADOR (orden=%d)", orden);
  }
  return 0;
}

orden_t ordenDePlanificador(void)
{
  orden_t *pOrden, orden;
  if( !recvWithBasicProtocol(socketPlanif, (void**)&pOrden) )
  {
    log_error(pLog, "No se pudo recibir la orden del planificador porque se desconecto.");
    exit(1);
  }
  orden = *pOrden;
  log_debug(pLog, "La orden recibida fue %s", orden==BLOQUEAR?"BLOQUEAR":orden==EJECUTAR?"EJECUTAR":"ABORTAR" );

  free(pOrden);

  return orden;
}

/*------SENTENCIAS--------*/

FILE * abrirScriptESI(char * ESIScriptFilename)
{
  FILE * scriptf = fopen(ESIScriptFilename, "r");
  if (scriptf == NULL)
  {
    log_error(pLog, "Error al abrir el archivo %s: %s", ESIScriptFilename, strerror(errno));
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
    pSent->valor = malloc(strlen(valor)+1);
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

  addSentenciaToBuffer(pBuffer,pSent);

  return pBuffer;
}

void addSentenciaToBuffer(tBuffer * pBuffer, ESISentenciaParseada_t * pSent)
{
  addIntToBuffer(pBuffer, pSent->operacion);
  addStringToBuffer(pBuffer, pSent->clave);
  if(pSent->operacion == SET)
    addStringToBuffer(pBuffer, pSent->valor);
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
