#ifndef ALGORITMOS_PLANIF_H
#define ALGORITMOS_PLANIF_H

#include <stdio.h>
#include <stdlib.h>
#include "../shared/protocolo.h"
#include <commons/collections/queue.h>
#include "ESIHandling/ESIHandling.h"
#include "consolaPlanificador.h"
#include <commons/config.h>


struct tipoPlanificacion{
	algoritmoPlanificacion planificacion;
	int alpha;
	float estimacionInicial;
}tipoPlanificacion;

typedef ESI_t*(*fDePlanif)(void);

ESI_t* obtenerEsiAEjecutarSegunFIFO();

ESI_t* obtenerEsiAEjecutarSegunSJF();

ESI_t* obtenerEsiAEjecutarSegunHHRR();

void planificarSegun(fDePlanif proximoESIAEjecutar);

float algoritmoDeEstimacionProximaRafaga(ESI_t* esi);

bool condicionParaListSort(ESI_t* esi_1, ESI_t* esi_2);

void ejecutarProxSent(ESI_t * pESI);

// ordena la cola y elige el siguiente ESI a ejecutar
#endif
