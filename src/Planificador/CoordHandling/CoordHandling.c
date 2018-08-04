/*------------PLANIFICADOR------------*/
#include "CoordHandling.h"

void atenderCoordinador( int socket )
{
  while(1)
  {
    int size;
    void * solicitud = NULL;

    size = recvWithBasicProtocol( socket, &solicitud);

    if( size ) // SI NO SE DESCONECTO
      procesarSolicitudCoordinador( solicitud, size);
    else
      coordinadorDesconectado();

    free(solicitud);
  }
}

void procesarSolicitudCoordinador(void * solicitud, int size)
{
  tBuffer * buffAviso = makeBuffer(solicitud, size);
  int idESI = readIntFromBuffer(buffAviso);
  int operacion = readIntFromBuffer(buffAviso);
  char * clave = readStringFromBuffer(buffAviso);

  log_trace(pLog, "El Coordinador pregunta si el ESI %d puede ejecutar\n"
                    "\tLa operacion %s sobre la clave %s\n",
                    idESI, operacion==0?"GET":operacion==1?"SET":operacion==2?"STORE":"???", clave);

  rtdoEjec_t rta = FAILURE;

  if( puedeEjecutar(idESI, operacion, clave) )
    rta = SUCCESS;
  else if( !buscarElementoDeLista(clave) )
    rta = ABORTED;

  sendWithBasicProtocol(socketCoord, &rta, sizeof(rtdoEjec_t));
  log_trace(pLog, "respuesta enviada al coordinador: %s",rta==FAILURE?"FAILURE":rta==SUCCESS?"SUCCESS":rta==ABORTED?"ABORTED":"???");
}

bool puedeEjecutar(int idESI, int op, char * clave)
{
  ESI_t* esi = NULL;
  cola_clave* c=NULL;
  if(claveEstaBloqueada(clave)){
    c = new_cola_clave(clave, 0);
    esi = buscarProcesoESI(idESI);
    queue_push(c->cola, esi);
    pthread_mutex_lock(&m_colaBloqueados);
    queue_push(ESIsBloqueados, esi);
    pthread_mutex_unlock(&m_colaBloqueados);
    if(op==GET){
      establecerOperacionPendiente(esi,GET,clave);
    }
    pthread_mutex_lock(&m_listaColas);
    list_add(ListaColas, c);
    pthread_mutex_unlock(&m_listaColas);
      return false;
  }
  else{ 
  c = buscarElementoDeLista(clave);
  if(op == GET)
  { // operacion GET
    esi = buscarProcesoESI(idESI);
    if(!claveBloqueadaParaESI(clave,esi)){ 
      if( c == NULL )
      {
        pthread_mutex_lock(&m_listaColas);
        list_add(ListaColas, new_cola_clave(clave, idESI));
        pthread_mutex_unlock(&m_listaColas);
        return true;
      }
      else
      {   
          if(c->idEsiUsandoClave != 0 && c->idEsiUsandoClave != idESI){ 
            queue_push(c->cola,esi);
            pthread_mutex_lock(&m_colaBloqueados);
            queue_push(ESIsBloqueados, esi);
            pthread_mutex_unlock(&m_colaBloqueados);
            establecerOperacionPendiente(esi,GET,clave);
            return false;
          }
          else{
            c->idEsiUsandoClave = idESI;
            return true;
          }     
      }
    }
    else{
        pthread_mutex_lock(&m_colaBloqueados);
        queue_push(ESIsBloqueados, esi);
        pthread_mutex_unlock(&m_colaBloqueados);
        establecerOperacionPendiente(esi,GET,clave);
        return false;
    }
  }
  else if(op == SET)
  { //operacion SET
      if(c != NULL){ 
        int idEsiConClave = c -> idEsiUsandoClave;
        return idEsiConClave == idESI;
      }
      else{
        return false;
      }
  }
  else
  { //operacion STORE
    if(c != NULL)
    {
          int idEsiConClave = c -> idEsiUsandoClave;
      if(idEsiConClave == idESI)
      {
            c->idEsiUsandoClave = 0;
            if(!queue_is_empty(c->cola)){
                idEsiConClave = ( (ESI_t*)(queue_pop(c->cola)) )->id;
                esi = buscarProcesoESI(idEsiConClave);
   
                esiADesbloquear = esi;
                pthread_mutex_lock(&m_colaBloqueados);
                list_remove_by_condition(ESIsBloqueados->elements, (void*) condicionRemover);
                pthread_mutex_unlock(&m_colaBloqueados);

                pthread_mutex_lock(&m_colaListos);
                queue_push(ESIsListos, esi);
                pthread_mutex_unlock(&m_colaListos);
                return true; 
            }
            else{
              return true;
            }
      }
      else{
        return false;
      }
    } 
    else{
      return false;
    } 
  }
  }
}

void coordinadorDesconectado()
{
  socketCoord = -1;
  log_error(pLog, "Se desconecto el Coordinador. El sistema se encuentra en un estado invalido.\nSaliendo del sistema.");
  exit(1);
}

bool condicionRemover(ESI_t* esi){
    return esi->id == esiADesbloquear->id;
}

void destruirElementoDeListaColas(cola_clave* c){
  claveAVerSiSatisfaceCondicionListaColas = malloc(strlen(c->clave)+1);
  claveAVerSiSatisfaceCondicionListaColas = c->clave;
  list_remove_by_condition(ListaColas, (void*)&condicionRemoverElementoDeListaColas);
  free(claveAVerSiSatisfaceCondicionListaColas);
}

bool condicionRemoverElementoDeListaColas(cola_clave* c){
    return !strcmp(claveAVerSiSatisfaceCondicionListaColas, c->clave);
}