#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

float algoritmoDeEstimacionProximaRafaga(ESI_t* esi){
	float valorSigRafaga= (((float)tipoPlanificacion->alpha)/100)*esi->duracionAnterior + (1-((float)tipoPlanificacion->alpha)/100)*esi->estimacionAnterior;
	return valorSigRafaga;
} //BIEN

bool condicionParaListSortSJF(ESI_t* esi_1, ESI_t* esi_2){
	float estimacionEsi_1 = algoritmoDeEstimacionProximaRafaga(esi_1);
	float estimacionEsi_2 = algoritmoDeEstimacionProximaRafaga(esi_2);

	return estimacionEsi_1 <= estimacionEsi_2;
} //BIEN

float calcularRatio(ESI_t* pEsi){
	float proximaRafaga = algoritmoDeEstimacionProximaRafaga(pEsi); 
	int esperaCPU = pEsi->tiempoEsperandoCPU;
	float ratio = ( (float)esperaCPU + proximaRafaga ) / proximaRafaga;
	return ratio;
} //BIEN

bool condicionParaListSortHRRN(ESI_t* esi_1, ESI_t* esi_2){
	float ratioEsi_1 = calcularRatio(esi_1);
	float ratioEsi_2 = calcularRatio(esi_2);

	return ratioEsi_1 >= ratioEsi_2;
} //BIEN

void ejecutarProxSent(ESI_t * pESI){
	orden_t orden = EJECUTAR;
	sendWithBasicProtocol(pESI->socket, &orden, sizeof(orden_t));
	pESI->tiempoEsperandoCPU = 0;
	pESIEnEjecucion = pESI;
} //BIEN

void planificarSegunFIFO(){
	ESI_t* pEsiAEjecutar;

	while(1){
			if(queue_size(ESIsListos) == 0){
				log_trace(pLog, "Se espera a que haya ESIs en la cola de listos");
			}
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunFIFO();
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

			switch( rtdoEjecucion )
			{
				case FAILURE:
					log_error(pLog, "El ESI de id = %d ha fallado");
					queue_push(ESIsBloqueados, pESIEnEjecucion);
					break;
				case NO_HAY_INSTANCIAS_CONECTADAS:
					log_error(pLog, "No hay instancias conectadas");
					abortESI(pESIEnEjecucion);
					break;
				case FIN_DE_EJECUCION:
					queue_push(ESIsFinalizados, pESIEnEjecucion);
					finalizarESI(pESIEnEjecucion);
					log_trace(pLog, "El ESI de id = %d pasa a finalizados", pESIEnEjecucion->id);
					break;
				case DISCONNECTED:
					log_trace(pLog, "El ESI de id = %d se desconecto inesperadamente", pESIEnEjecucion->id);
					break;
				default:
					log_error(pLog, "ERROR: rtdoEjecucion desconocido");
			}
	}
}

void planificarSegunSJF(){
	ESI_t* pEsiAEjecutar;
	int duracion = 0;

	while(1){
			if(queue_size(ESIsListos) == 0){
				log_trace(pLog, "Se espera a que haya ESIs en la cola de listos");
			}
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunSJF();
			log_trace(pLog, "Segun SJF el ESI a ejecutar ahora es el de id = %d", pEsiAEjecutar->id);
			pESIEnEjecucion = pEsiAEjecutar;

			do{
			pthread_mutex_lock(&m_puedeEjecutar);
			ejecutarProxSent(pESIEnEjecucion);
			duracion++;
			pthread_mutex_unlock(&m_puedeEjecutar);
			log_trace(pLog, "Se dio la orden de ejecutar al ESI en ejecucion");

			log_trace(pLog,"Se espera la respuesta del ESI en ejecucion");
			sem_wait(&sem_respuestaESI);
		    }
			while(rtdoEjecucion == SUCCESS);
			pESIEnEjecucion->duracionAnterior = duracion;

			if( rtdoEjecucion == FAILURE){
			log_error(pLog, "El ESI de id = %d ha fallado");
			queue_push(ESIsBloqueados, pESIEnEjecucion); 
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

void planificarSegunSRT(){
	ESI_t* pEsiAEjecutar;
	int duracion = 0;

	while(1){

			if(queue_size(ESIsListos) == 0){
				log_trace(pLog, "Se espera a que haya ESIs en la cola de listos");
			}
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunSJF();
			log_trace(pLog, "Segun SRT el ESI a ejecutar ahora es el de id = %d", pEsiAEjecutar->id);
			pESIEnEjecucion = pEsiAEjecutar;

			int sizeColaReadyAntesDeEjecutar = queue_size(ESIsListos);

			do{
				pthread_mutex_lock(&m_puedeEjecutar);
				ejecutarProxSent(pESIEnEjecucion);
				duracion++;
				pthread_mutex_unlock(&m_puedeEjecutar);
				log_trace(pLog, "Se dio la orden de ejecutar al ESI en ejecucion");

				log_trace(pLog,"Se espera la respuesta del ESI en ejecucion");
				sem_wait(&sem_respuestaESI);
			}
			while( rtdoEjecucion == SUCCESS && queue_size(ESIsListos) == sizeColaReadyAntesDeEjecutar );
			pESIEnEjecucion->duracionAnterior = duracion;

			if( rtdoEjecucion == FAILURE){
			log_error(pLog, "El ESI de id = %d ha fallado");
			queue_push(ESIsBloqueados, pESIEnEjecucion); 
			}
			else
				if( rtdoEjecucion == NO_HAY_INSTANCIAS_CONECTADAS ){
					log_error(pLog, "No hay instancias conectadas");
					abortESI(pESIEnEjecucion);
				}
				else
					if( rtdoEjecucion == FIN_DE_EJECUCION ){
						queue_push(ESIsFinalizados, pESIEnEjecucion);
						finalizarESI(pESIEnEjecucion);
						log_trace(pLog, "El ESI de id = %d pasa a finalizados", pESIEnEjecucion->id);
					}
					else
						if(rtdoEjecucion == SUCCESS){
							list_add_in_index( (t_list *)ESIsListos, 0, (void *)pEsiAEjecutar ); // para que si empata con otro esi, se aplique la regla FIFO
						}
						else
							log_error(pLog, "ERROR: rtdoEjecucion desconocido");

			// si rtdoEjecucion = DISCONNECTED no hace nada y sigue planificando
	}
}

void planificarSegunHRRN(){
	ESI_t* pEsiAEjecutar;
	int duracion = 0;

	while(1){
			if(queue_size(ESIsListos) == 0){
				log_trace(pLog, "Se espera a que haya ESIs en la cola de listos");
			}
			sem_wait(&sem_cantESIsListos);//if(!queue_is_empty(ESIsListos))	//solo planifica si hay ESIs que planificar

			pEsiAEjecutar = obtenerEsiAEjecutarSegunHRRN();
			log_trace(pLog, "Segun HRRN el ESI a ejecutar ahora es el de id = %d", pEsiAEjecutar->id);
			pESIEnEjecucion = pEsiAEjecutar;

			do{
			pthread_mutex_lock(&m_puedeEjecutar);
			ejecutarProxSent(pESIEnEjecucion);
			duracion++;
			pthread_mutex_unlock(&m_puedeEjecutar);
			log_trace(pLog, "Se dio la orden de ejecutar al ESI en ejecucion");

			log_trace(pLog,"Se espera la respuesta del ESI en ejecucion");
			sem_wait(&sem_respuestaESI);
		    }
			while(rtdoEjecucion == SUCCESS);
			pESIEnEjecucion->duracionAnterior = duracion;

			if( rtdoEjecucion == FAILURE){
			log_error(pLog, "El ESI de id = %d ha fallado");
			queue_push(ESIsBloqueados, pESIEnEjecucion); 
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

ESI_t*  obtenerEsiAEjecutarSegunFIFO(){
    ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	return pEsiAEjecutar;
} //BIEN

ESI_t*  obtenerEsiAEjecutarSegunSJF(){
	list_sort( ESIsListos->elements, (void*) condicionParaListSortSJF );
	ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	pEsiAEjecutar->estimacionAnterior = algoritmoDeEstimacionProximaRafaga(pEsiAEjecutar);
	pEsiAEjecutar->duracionAnterior = 0;
	return pEsiAEjecutar;
} //BIEN

ESI_t* obtenerEsiAEjecutarSegunHRRN(){
	list_sort( ESIsListos->elements, (void*) condicionParaListSortHRRN );
	ESI_t* pEsiAEjecutar = queue_pop(ESIsListos);
	pEsiAEjecutar->estimacionAnterior = algoritmoDeEstimacionProximaRafaga(pEsiAEjecutar);
	pEsiAEjecutar->duracionAnterior = 0;
	pEsiAEjecutar->tiempoEsperandoCPU = 0;
	return pEsiAEjecutar;
} //BIEN

void obtenerEstructuraDePlanificacion(t_config * pConf)
{
	char * planifString =config_get_string_value(pConf, "ALGORITMO_DE_PLANIFICACION");
	if( !strcmp(planifString, "FIFO") )
	{
		tipoPlanificacion->planificacion = FIFO;
		tipoPlanificacion->alpha = 0;
  		tipoPlanificacion->estimacionInicial = 0;
	}
	else
	{
		if(!strcmp(planifString, "SJF")){
			tipoPlanificacion->planificacion = SJF;
		}
		else{
			if(!strcmp(planifString, "SRT")){
				tipoPlanificacion->planificacion = SRT;
			}
			else{
				tipoPlanificacion->planificacion = HRRN;
			}
		}
		tipoPlanificacion->alpha = config_get_int_value(pConf, "ALPHA");
  		tipoPlanificacion->estimacionInicial = (float)config_get_int_value(pConf, "ESTIMACION_INICIAL");
	}
} //BIEN