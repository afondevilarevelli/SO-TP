#ifndef ALGORITMOS_PLANIF_H
#define ALGORITMOS_PLANIF_H

#include <stdio.h>
#include <stdlib.h>
#include "../shared/protocolo.h"
#include <commons/collections/queue.h>
#include "ESIHandling/ESIHandling.h"


struct tipoPlanificacion{
	algoritmoPlanificacion planificacion;
	int alpha;
	int estimacionInicial;
}tipoPlanificacion;

ESI_t* obtenerEsiAEjecutarSegunFIFO(t_queue* ESIsListos);

ESI_t* obtenerEsiAEjecutarSegunSFJ(t_queue* ESIsListos, struct tipoPlanificacion tPlani);

ESI_t* obtenerEsiAEjecutarSegunSRT(t_queue* ESIsListos, struct tipoPlanificacion tPlani);

ESI_t* obtenerEsiAEjecutarSegunHHRR(t_queue* ESIsListos);

// ordena la cola y elige el siguiente ESI a ejecutar
#endif
