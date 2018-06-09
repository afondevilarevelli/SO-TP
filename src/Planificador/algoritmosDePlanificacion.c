#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

float algoritmoDeEstimacionProximaRafaga(ESI_t* esi){
	float valorSigRafaga= ((tipoPlanificacion.alpha/100)*esi->duracionAnterior)+((1-(tipoPlanificacion.alpha/100))*esi->estimacionAnterior);
	return valorSigRafaga;
}

bool condicionParaListSort(ESI_t* esi_1, ESI_t* esi_2){
	float estimacionEsi_1 = algoritmoDeEstimacionProximaRafaga(esi_1);
	float estimacionEsi_2 = algoritmoDeEstimacionProximaRafaga(esi_2);

	if( estimacionEsi_1 <= estimacionEsi_2 ){
		return 1;
	}
	else{
		return 0;
	}
}

void ejecutarProxSent(ESI_t * pESI){
	orden_t orden = EJECUTAR;
	sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
	log_debug(pLog, "Se envio la orden = %s, al ESI de id = %d", orden==BLOQUEAR?"BLOQUEAR":orden==EJECUTAR?"EJECUTAR":"ABORTAR", pESI->id);
	pESIEnEjecucion = pESI;
}

void planificarSegun(fDePlanif proximoESIAEjecutar){
	ESI_t* pEsiAEjecutar;

	while(1)
	{

		log_trace(pLog, "Se espera a que haya ESIs en la cola de listos");
		sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

		pEsiAEjecutar = proximoESIAEjecutar();
		log_trace(pLog, "Segun FIFO el ESI a ejecutar ahora es el de id = %d", pEsiAEjecutar->id);
		pESIEnEjecucion = pEsiAEjecutar;

		do
		{
			pthread_mutex_lock(&m_puedeEjecutar);
			ejecutarProxSent(pESIEnEjecucion);
			pthread_mutex_unlock(&m_puedeEjecutar);
			log_trace(pLog, "Se dio la orden de ejecutar al ESI en ejecucion");

			log_trace(pLog,"Se espera la respuesta del ESI en ejecucion");
			sem_wait(&sem_respuestaESI);
		}
		while(rtdoEjecucion == SUCCESS);

		if( rtdoEjecucion == FAILURE){
			log_error(pLog, "El ESI de id = %d ha fallado");
			queue_push(ESIsBloqueados, pESIEnEjecucion); //debería ver porque se bloqueó el ESI
		}
		else if( rtdoEjecucion == NO_HAY_INSTANCIAS_CONECTADAS )
		{
			log_error(pLog, "No hay instancias conectadas");
			abortESI(pESIEnEjecucion);
		}
		else if( rtdoEjecucion == FIN_DE_EJECUCION )
		{
			queue_push(ESIsFinalizados, pESIEnEjecucion);
			finalizarESI(pESIEnEjecucion);
			log_trace(pLog, "El ESI de id = %d pasa a finalizados", pESIEnEjecucion->id);
		}
		else
			log_error(pLog, "ERROR: rtdoEjecucion desconocido");
		// si rtdoEjecucion = DISCONNECTED no hace nada y sigue planificando

	}
}

void planificarSegunHRRN(){
	return;
}

ESI_t*  obtenerEsiAEjecutarSegunFIFO(){
  ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	//printf("Listos tiene ahora %d elementos\n", ESIsListos->elements->elements_count);
	return pEsiAEjecutar;
}

ESI_t*  obtenerEsiAEjecutarSegunSJF(){
	list_sort( (t_list*) ESIsListos, (void*) condicionParaListSort );
	ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	pEsiAEjecutar->estimacionAnterior = algoritmoDeEstimacionProximaRafaga(pEsiAEjecutar);
	return pEsiAEjecutar;
}

ESI_t* obtenerEsiAEjecutarSegunHHRR(){
	return queue_pop(ESIsListos);
}
