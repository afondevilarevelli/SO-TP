#ifndef ALGORITMOS_PLANIF_H
#define ALGORITMOS_PLANIF_H

#include <stdio.h>
#include <stdlib.h>
#include "../shared/protocolo.h"
#include <commons/collections/queue.h>
#include "ESIHandling/ESIHandling.h"
#include "consolaPlanificador.h"
#include <commons/config.h>
#include <unistd.h>


typedef struct{
	algoritmoPlanificacion planificacion;
	int alpha;
	float estimacionInicial;
}tipoPlanif;

tipoPlanif* tipoPlanificacion;

int idEsiParaRemoverDeCola;

ESI_t* obtenerEsiAEjecutarSegunFIFO();

ESI_t* obtenerEsiAEjecutarSegunSJF();

ESI_t* obtenerEsiAEjecutarSegunHRRN();

void planificarSegunFIFO();

void planificarSegunSJF();

void planificarSegunSRT();

void planificarSegunHRRN();

float algoritmoDeEstimacionProximaRafaga(ESI_t* esi);

bool condicionParaListSortSJF(ESI_t* esi_1, ESI_t* esi_2);

bool condicionParaListSortHRRN(ESI_t* esi_1, ESI_t* esi_2);

void ejecutarProxSent(ESI_t * pESI);

void obtenerEstructuraDePlanificacion(t_config * pConf);

float calcularRatio(ESI_t* pEsi);

bool condicionParaRemoverDeLaCola(ESI_t* e);

// ordena la cola y elige el siguiente ESI a ejecutar
#endif
