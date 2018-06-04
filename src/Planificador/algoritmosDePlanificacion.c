#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

int algoritmoDeEstimacionProximaRafaga(ESI_t* esi, struct tipoPlanificacion tPlani, int valorReal);
//t_queue colaESIs = queue_create(); //cola de ID de ESI's

ESI_t*  obtenerEsiAEjecutarSegunFIFO(t_queue* ESIsListos){
    return queue_pop(ESIsListos);
}

ESI_t*  obtenerEsiAEjecutarSegunSJF(t_queue* ESIsListos, struct tipoPlanificacion tPlani){
	return queue_pop(ESIsListos);
}

ESI_t* obtenerEsiAEjecutarSegunSRT(t_queue* ESIsListos, struct tipoPlanificacion tPlani, int valorReal){

	int size=queue_size(ESIsListos)-1;

	ESI_t* esi_1=queue_pop(ESIsListos);
	ESI_t* esi_2=NULL;
	int estimacionActual_1 =algoritmoDeEstimacionProximaRafaga( esi_1, tPlani, valorReal);
	int estimacionActual_2;
	for(int i=0;size!=i;i++){
		esi_2=queue_pop(ESIsListos);
		 estimacionActual_2 =algoritmoDeEstimacionProximaRafaga( esi_2, tPlani, valorReal);
		if( estimacionActual_1<estimacionActual_2){
			queue_push(ESIsListos,esi_2);
		}else{
			queue_push(ESIsListos,esi_1);
			esi_1=esi_2;
			estimacionActual_1=estimacionActual_2;
		}
	}

	esi_1->estimacionRafaga=estimacionActual_1;
	return esi_1;
}

ESI_t* obtenerEsiAEjecutarSegunHHRR(t_queue* ESIsListos){
	return queue_pop(ESIsListos);
}


int algoritmoDeEstimacionProximaRafaga(ESI_t* esi, struct tipoPlanificacion tPlani, int valorReal){
	int valorSigRafaga= ((tPlani.alpha/100)*valorReal)+((1-(tPlani.alpha/100))*esi->estimacionRafaga);
	return valorSigRafaga;
}
