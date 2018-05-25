#ifndef CONSOLA_PLANIFICADOR_H
#define CONSOLA_PLANIFICADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>//importar librearias

typedef enum{PAUSADO, PLANIFICANDO} est_Planif;

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
