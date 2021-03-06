#include "ESIHandling.h"
/*--------NUEVOS ESIS--------*/

void finalizarESI(ESI_t * pESI)
{
  pESI->state = FINALIZADO;
  pESI->operacionPendiente->operacion=ORDEN_COMPACTAR;

}

int fueFinalizado(ESI_t * pESI)
{
  return pESI->state == FINALIZADO;
}

void atenderESI(ESI_t * pESI)
{
  while(!fueAbortado(pESI) && !fueFinalizado(pESI))
  {
    int size;
    void * rtdoEjec;

    size = recvWithBasicProtocol(pESI->socket, &rtdoEjec);

    if( size ) // SI NO SE DESCONECTO
    {
      rtdoEjecucion = *((rtdoEjec_t*)rtdoEjec);
      sem_post(&sem_respuestaESI);
    }
    else
    {
      ESIDesconectado( pESI->id );
      break;
    }
  }

  if(fueAbortado(pESI)){ 
    eliminarESIDelSistema(pESI->id); }
  else{ 
    if(pESI->state != MATADO){
      sem_wait(&sem_esperarFinalizarESI);
      log_warning(pLog, "El ESI con ID = %d finalizo y no atendera mas solicitudes", pESI->id);   
    }
    else{ // si fue matado
      if(pESIEnEjecucion->id == pESI->id){
        rtdoEjecucion = 10;
		    sem_post(&sem_respuestaESI);
      }
      freeESI(pESI);
    }
  }
}

void abortESI(ESI_t * pESI)
{
  pESI->state = ABORTADO;
  orden_t orden = ABORTAR;
  log_error(pLog, "El ESI con ID = %d, fue abortado\n", pESI->id);
  sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
}

void matarESI(ESI_t* pESI){
  pESI->state = MATADO;
  orden_t orden = ABORTAR;
  log_error(pLog, "El ESI con ID = %d, fue matado\n", pESI->id);
  sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
}

int fueAbortado(ESI_t * pESI)
{
  return pESI->state == ABORTADO;
}

void ESIDesconectado( int ESI_ID )
{
  log_warning(pLog, "El ESI con ID = %d, se desconecto\n", ESI_ID);
}

ESI_t * quitarESIDeSuListaActual(int ESI_ID)
{
  ESI_t * pESI = NULL;
  if( pESIEnEjecucion && is_ESI_ID_equal(pESIEnEjecucion, ESI_ID) )
    pESI = pESIEnEjecucion;

  if(!pESI)
  {
    pthread_mutex_lock(&m_colaListos);
    pESI = get_and_remove_ESI_by_ID( ESIsListos->elements, ESI_ID);
    pthread_mutex_unlock(&m_colaListos);
    if(pESI) sem_wait(&sem_cantESIsListos);
  }

  if(!pESI)
  {
    pthread_mutex_lock(&m_colaBloqueados);
    pESI = get_and_remove_ESI_by_ID( ESIsBloqueados->elements, ESI_ID);
    pthread_mutex_unlock(&m_colaBloqueados); }

    if(!pESI){
      pESI = get_and_remove_ESI_by_ID( ESIsFinalizados->elements, ESI_ID); }

  return pESI;
}

void eliminarESIDelSistema( int ESI_ID )
{
  ESI_t * pESI = quitarESIDeSuListaActual(ESI_ID);
  borrarEsiDeListaColas(pESI);

  freeESI(pESI);
  log_warning(pLog, "El ESI con ID = %d, fue eliminado del sistema\n", ESI_ID);
}

ESI_t * get_and_remove_ESI_by_ID( t_list * ESIs, int id )
{
  t_link_element * pAct = ESIs->head , * pPrev = NULL;
  ESI_t * pESI;

  while( pAct != NULL )
  {
    pESI = (ESI_t *)(pAct->data);

    if( is_ESI_ID_equal( pESI, id) )
    {
      if(!pPrev)
        ESIs->head = pAct->next;
      else
        pPrev->next = pAct->next;

      return pESI;
    }
    pPrev = pAct;
    pAct = pAct->next;
  }

  return NULL;
}

bool is_ESI_ID_equal( ESI_t * pESI, int id )
{
  return pESI->id == id;
}


ESI_t * newESI( int socket, int id, int rafagaInicial)
{
  ESI_t * pESI = malloc(sizeof(ESI_t));

  pESI->socket = socket;
  pESI->id = id;
  pESI->estimacionAnterior= rafagaInicial;
  pESI->duracionAnterior = rafagaInicial;
  pESI->state = NORMAL;
  pESI->tiempoEsperandoCPU = 0;
  pESI->operacionPendiente = malloc(sizeof(operacionESI)); 
  pESI->operacionPendiente->operacion = ORDEN_COMPACTAR; // le meto basura
  pESI->operacionPendiente->clave = NULL;

  return pESI;
}

void establecerOperacionPendiente(ESI_t* esi, op_t op, char* clave){ //solo voy a establecer las operaciones GET
  if(esi!=NULL){
    if(esi->operacionPendiente->clave != NULL){
      free(esi->operacionPendiente->clave);
    }
    esi->operacionPendiente->clave = malloc(strlen(clave)+1);
    esi->operacionPendiente->operacion = op;
    strcpy(esi->operacionPendiente->clave, clave);
  }
}

void freeESI(ESI_t * pESI)
{
  if(pESI->operacionPendiente->clave != NULL){
    free(pESI->operacionPendiente->clave);
  }
  free(pESI->operacionPendiente);
  free(pESI);
}

cola_clave* buscarElementoDeLista(char* clave){//busca de mi ListaColas la que se identifica con la clave
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if (!strcmp(c -> clave, clave)){
			return c;
		}
	p = p -> next ;
	}
	return NULL ;
}

cola_clave* new_cola_clave(char * clave, int idESI)
{
  cola_clave * pCClave = malloc(sizeof(cola_clave));
  pCClave->clave = malloc(strlen(clave)+1);
  strcpy(pCClave->clave, clave);
  pCClave->idEsiUsandoClave = idESI;
  pCClave->cola = queue_create();
  pCClave->esisBloqueadosParaClave = list_create();

  return pCClave;
}

bool claveEstaBloqueada(char* clave){
  int estaBloqueada;
  claveAVerSiSatisface = malloc(sizeof(char)*40);
  strcpy(claveAVerSiSatisface, clave);
  estaBloqueada = list_count_satisfying(clavesBloqueadas, (void*) condicionSatisfy);
  free(claveAVerSiSatisface);
  return estaBloqueada > 0;
}

bool condicionSatisfy(void* cl){
    int result = strcmp(claveAVerSiSatisface, cl);
    return result == 0;
}

void bloquearClaves(t_config* conf){
	int i = 1;
	int posicion = 14;
	char claveBloqueada[] = "claveBloqueadaX";
	claveBloqueada[posicion] = i + '0';
	char* clave = config_get_string_value(conf, claveBloqueada);
	while(clave != NULL){
		list_add(clavesBloqueadas, (void*)clave );
    log_trace(pLog, "Se ha bloqueado la clave %s", clave);
		i++;
		claveBloqueada[posicion] = i + '0';	
		clave = config_get_string_value(conf, claveBloqueada);
	}
}

void sumarTiempoEsperandoCPU(int tiempo){
    tiempoParaIterar = tiempo;
    list_iterate(ESIsListos->elements, (void *)&closureParaIterar);
}

void closureParaIterar(ESI_t* esi){
    esi->tiempoEsperandoCPU += tiempoParaIterar;
}

bool claveBloqueadaParaESI(char* clave, ESI_t* esi){
    cola_clave* c = buscarElementoDeLista(clave);
    if(c!=NULL){ 
      esiAVerSiEstaBloqueado = esi;
      return list_any_satisfy(c->esisBloqueadosParaClave, (void *)&closureSatisfyBlock);
    }
    else{
      return false;
    }
}

bool closureSatisfyBlock(ESI_t* esi){
    return esiAVerSiEstaBloqueado->id == esi->id;
}

bool estaBloqueadoPorOtraClave(ESI_t* esi){
  bool booleano;
  idAVerSiSatisfaceBloqueo = esi->id;
  booleano = list_any_satisfy(ListaColas, (void*)&satisfaceBloqueo);
  return booleano;
}

bool satisfaceBloqueo(cola_clave* c){
  return list_any_satisfy((c->cola)->elements, (void*)&satisfaceBloqueoANivelCola ) && c->idEsiUsandoClave != 0;
}

bool satisfaceBloqueoANivelCola(ESI_t* esi){
  return idAVerSiSatisfaceBloqueo == esi->id;
}

void borrarEsiDeListaColas(ESI_t* esi){
    esiParaEliminarDeListaColas = esi;
    list_iterate(ListaColas, (void*)&closureIterateBorrado);
}

void closureIterateBorrado(cola_clave* c){
    if(c->idEsiUsandoClave == esiParaEliminarDeListaColas->id){
      c->idEsiUsandoClave = 0;
      if(!queue_is_empty(c->cola)){
                int idEsiConClave = ( (ESI_t*)(queue_pop(c->cola)) )->id;
                ESI_t* esi = buscarProcesoESI(idEsiConClave);
   
                esiADesbloquearPorMatado = esi;
                pthread_mutex_lock(&m_colaBloqueados);
                list_remove_by_condition(ESIsBloqueados->elements, (void*) condicionRemoverAlMatado);
                pthread_mutex_unlock(&m_colaBloqueados);

                pthread_mutex_lock(&m_colaListos);
                queue_push(ESIsListos, esi);
                pthread_mutex_unlock(&m_colaListos);
                sem_post(&sem_cantESIsListos);
    }
    list_remove_by_condition((c->cola)->elements, (void*)&condicionEliminarEsi);
    list_remove_by_condition(c->esisBloqueadosParaClave, (void*)&condicionEliminarEsi);
}
}

bool condicionEliminarEsi(ESI_t* esi){
  return esi->id == esiParaEliminarDeListaColas->id;
}

bool condicionRemoverAlMatado(ESI_t* e){
  return e->id == esiADesbloquearPorMatado->id;
}

ESI_t* buscarProcesoESI(int id){// busca en el sistema en la lista de listos y si el proceso esta ejecutando
		if(pESIEnEjecucion != NULL){ 
			if(pESIEnEjecucion->id == id){
				return pESIEnEjecucion;
			}
		}
		ESI_t* p = buscarProcesoEnColas(ESIsListos,id);
		if(p != NULL ){
			return p;
		}
		else{
			p = buscarProcesoEnColas(ESIsBloqueados, id);
			if(p!=NULL){
				return p;
			}
		}
		return p;
}

ESI_t* buscarProcesoEnColas(t_queue* cola,int id){
	ESI_t* p ;
	t_link_element* pElem = (cola -> elements) -> head ; //
	while(pElem != NULL){

		p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		if(id == p->id){
		return p;
		}
		pElem = pElem->next;
	}
	return NULL;// SI NO ESTA EN LA cola BOTA NULL la funcion
}
/*----------CONEXIONES---------*/

void recibirNuevosESI(t_config * pConf)
{
  int ip, puerto;
  obtenerIPyPuertoDePlanificador(pConf, &ip, &puerto);

  int listener = listenOn(ip, puerto);

  fd_set masterSet, readSet;
  FD_ZERO(&masterSet);
  FD_SET(listener, &masterSet);

  int maxFD = listener, i;
  while(1)
  {
    readSet = masterSet;
    select(maxFD + 1, &readSet, NULL, NULL, NULL);

    atenderConexionEntrante(listener,config_get_int_value(pConf, "ESTIMACION_INICIAL"));
  }
}

void atenderConexionEntrante(int listener, int estimacionInicialESI)
{
  int newSock = acceptClient(listener);
  void * pID = NULL;
  if(recvWithBasicProtocol(newSock, &pID))
  {
    ESI_t * pESI = newESI(newSock, *((int*)pID),estimacionInicialESI );
    //log_trace(pLog, "Atendiendo ESI de ID=%d", *(int*)pID);
    pthread_mutex_lock(&m_colaListos);
    queue_push(ESIsListos, pESI);
    pthread_mutex_unlock(&m_colaListos);
    sem_post(&sem_cantESIsListos);

    pthread_t hiloESI;
    pthread_create(&hiloESI, NULL, (void*)&atenderESI, (void*)pESI);
    //agregare el hilo a la lista global de hilos del coordinador
    list_add(hilos, (void*)&hiloESI);

    free(pID);
  }
  else{
    perror("no se pudo atender la conexion entrante");
    exit(1);
  }
}

void terminarHilo( pthread_t * pHilo )
{
  pthread_join(*pHilo, NULL);
}
