#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

ESI_t*  obtenerEsiAEjecutarSegunFIFO(){
	pthread_mutex_lock(&mutex_colaReady);
    ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	pthread_mutex_unlock(&mutex_colaReady);
	return pEsiAEjecutar;
}

ESI_t*  obtenerEsiAEjecutarSegunSJF(){
	pthread_mutex_lock(&mutex_colaReady);
	list_sort( (t_list*) ESIsListos, (void*) condicionParaListSort );
	ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	pEsiAEjecutar->estimacionAnterior = algoritmoDeEstimacionProximaRafaga(pEsiAEjecutar); 
	pthread_mutex_unlock(&mutex_colaReady);
	return pEsiAEjecutar;
}

ESI_t* obtenerEsiAEjecutarSegunSRT(){

	int size=queue_size(ESIsListos)-1;

	ESI_t* esi_1=queue_pop(ESIsListos);
	ESI_t* esi_2=NULL;
	int estimacionActual_1 =algoritmoDeEstimacionProximaRafaga( esi_1);
	int estimacionActual_2;
	for(int i=0;size!=i;i++){
		esi_2=queue_pop(ESIsListos);
		 estimacionActual_2 =algoritmoDeEstimacionProximaRafaga( esi_2);
		if( estimacionActual_1<estimacionActual_2){
			queue_push(ESIsListos,esi_2);
		}else{                                                      //falta aplicar el mutex para colaReady
			queue_push(ESIsListos,esi_1);
			esi_1=esi_2;
			estimacionActual_1 = estimacionActual_2;
		}
	}

	esi_1->estimacionAnterior = estimacionActual_1;
	return esi_1;
}

ESI_t* obtenerEsiAEjecutarSegunHHRR(){
	return queue_pop(ESIsListos);
}


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
}

void planificarSegunFifo(){ 
	ESI_t* pEsiAEjecutar; 

	while(1){

		while(puedeEjecutar()){
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar
			pthread_mutex_lock(&m_ESIEjecutandose);

			pEsiAEjecutar = obtenerEsiAEjecutarSegunFIFO();
			ejecutarProxSent(&pEsiAEjecutar); 
			void* buffer = malloc(sizeof(rtdoEjec_t));
			if( revWithBasicProtocol(pEsiAEjecutar->socket, &buffer) == -1 ){
					perror("Esi con id = %d desconectado",pEsiAEjecutar->id);
					exit(1);
			}
			while( (int ) *buffer == SUCCESS ){
				ejecutarProxSent(&pEsiAEjecutar);
				if( revWithBasicProtocol(pEsiAEjecutar->socket, &buffer) == -1 ){
						perror("Esi con id = %d desconectado",pEsiAEjecutar->id);
						exit(1);
				}	
			}
		}
	}
}