#ifndef CONSOLA_PLANIFICADOR_H
#define CONSOLA_PLANIFICADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include "ESIHandling/ESIHandling.h"//importar librearias

typedef enum{PAUSADO, PLANIFICANDO} est_Planif;




//bloquear el proceso ESI por consola
//no olvidar poner la libreria <commons/collection/queue.h>  de las colas
// yo puse al tipo de dato esi o proceso esi -> le puse* pro_ESI

//creo una structura para tipo de dato ESI
typedef struct {
	int id_esi;
	t_cola colaProviniente;
}pro_ESI;


void pausarPlanificacion();
void continuarPlanificacion();
void bloquearProcesoESI();//no se si se le pasa un parametro
void desbloquearProcesoESI();//desbloquear
void listarProcesos();//listar
void finalizarProceso();//kill
void informacionDeInstancias();//status
void analizarDeadlockDelSistema();//deadlock
void consolaPlanificador();
int puedeEjecutar();




#endif
