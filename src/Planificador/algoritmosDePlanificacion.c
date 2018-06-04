#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

ESI_t*  algoritmoDeEstimacionProximaRafaga(ESI_t esi,struct tipoPlanificacion tPlani);
//t_queue colaESIs = queue_create(); //cola de ID de ESI's

ESI_t*  obtenerEsiAEjecutarSegunFIFO(t_queue* ESIsListos){
    return queue_pop(ESIsListos);
}

ESI_t*  obtenerEsiAEjecutarSegunSJF(t_queue* ESIsListos, struct tipoPlanificacion tPlani){
	return queue_pop(ESIsListos);
}

ESI_t* obtenerEsiAEjecutarSegunSRT(t_queue* ESIsListos, struct tipoPlanificacion tPlani){
	return queue_pop(ESIsListos);
}

ESI_t* obtenerEsiAEjecutarSegunHHRR(t_queue* ESIsListos){
	return queue_pop(ESIsListos);
}


ESI_t* algoritmoDeEstimacionProximaRafaga(ESI_t esi, struct tipoPlanificacion tPlani){
	return queue_pop(ESIsListos);
}
