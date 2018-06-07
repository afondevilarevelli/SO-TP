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
	pESIEnEjecucion = pESI;
}

void planificarSegunFifo(){ 
	ESI_t* pEsiAEjecutar; 

	while(1){

		while(puedeEjecutar()){
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunFIFO();
			ejecutarProxSent(pEsiAEjecutar);
			pESIEnEjecucion = pEsiAEjecutar;
			sem_wait(&sem_respuestaESI);
			while(*(rtdoEjecucion) == SUCCESS){
				ejecutarProxSent(pEsiAEjecutar);
				pESIEnEjecucion = pEsiAEjecutar;
				sem_wait(&sem_respuestaESI);
			}
			if( *(rtdoEjecucion) == FAILURE){
				queue_push(ESIsBloqueados, pEsiAEjecutar); //debería ver porque se bloqueó el ESI
			}
			else{ //rtdoEjecucion = FIN_DE_EJECUCION
				queue_push(ESIsFinalizados, pEsiAEjecutar);
			}
			// si rtdoEjecucion = DISCONNECTED no hace nada y sigue planificando		
		}
	}
}

//VER EL ALGORITMO QUE SE USÓ PARA planificarSegunFIFO()
void planificarSegunSJF(){
	ESI_t* pEsiAEjecutar; 

	while(1){

		while(puedeEjecutar()){
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunSJF();
			ejecutarProxSent(pEsiAEjecutar);
			sem_wait(&sem_respuestaESI);
			while(*(rtdoEjecucion) == SUCCESS){
				ejecutarProxSent(pEsiAEjecutar);
				sem_wait(&sem_respuestaESI);
			}
			if( *(rtdoEjecucion) == FAILURE){
				queue_push(ESIsBloqueados, pEsiAEjecutar); //debería ver porque se bloqueó el ESI
			}
			else{ //rtdoEjecucion = FIN_DE_EJECUCION
				queue_push(ESIsFinalizados, pEsiAEjecutar);
			}
			// si rtdoEjecucion = DISCONNECTED no hace nada y sigue planificando		
		}
	}
}//Tengo q pensarlo bien si esta bien la logica

//VER EL ALGORITMO QUE SE USÓ PARA planificarSegunFIFO()
void planificarSegunSRT(){
	ESI_t* pEsiAEjecutar;

	while(1){

		while(puedeEjecutar()){
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunSJF();
			ejecutarProxSent(pEsiAEjecutar);
			int sizeColaReadyAntesDeEjecutar = queue_size(ESIsListos);

			sem_wait(&sem_respuestaESI);
			while(*(rtdoEjecucion) == SUCCESS && queue_size(ESIsListos) == sizeColaReadyAntesDeEjecutar ){
				ejecutarProxSent(pEsiAEjecutar);
				sizeColaReadyAntesDeEjecutar = queue_size(ESIsListos);
				sem_wait(&sem_respuestaESI);
			}
			if( *(rtdoEjecucion) == FAILURE){
				queue_push(ESIsBloqueados, pEsiAEjecutar); //debería ver porque se bloqueó el ESI
			}
			else{
				if(*(rtdoEjecucion) == FIN_DE_EJECUCION){  //rtdoEjecucion = FIN_DE_EJECUCION
				queue_push(ESIsFinalizados, pEsiAEjecutar);
				}
				else{
					if(*(rtdoEjecucion) == SUCCESS){
						list_add_in_index( (t_list *)ESIsListos, 0, (void *)pEsiAEjecutar ); // para que si empata con otro esi, se aplique la regla FIFO
					}
				}
			}
			// si rtdoEjecucion = DISCONNECTED no hace nada y sigue planificando
		}
	}
}

void planificarSegunHRRN(){
	return;
}

ESI_t*  obtenerEsiAEjecutarSegunFIFO(){
    ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
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

