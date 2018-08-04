#ifndef CONSOLA_PLANIFICADOR_H
#define CONSOLA_PLANIFICADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include "ESIHandling/ESIHandling.h"//importar librearias
#include "../Inst1/bibliotecaInstancia.h"
#include "CoordHandling/CoordHandling.h"


typedef enum{PAUSADO, PLANIFICANDO} est_Planif;
ESI_t* esiAVerSiDesbloqueo;
char* claveParaDesbloquearSiEstaBloqueada;

//funciones que pueden usar
t_queue* colaAsociada(char* clave);//busca en ListaColas la que se identifiqua con la clave y devuelve la cola
void pausarPlanificacion();
void continuarPlanificacion();

void bloquearProcesoESI(char* clave,int id);

void desbloquearProcesoESI(char* clave);

void listar(char* clave);

void finalizarProceso();//kill
void informacionDeInstancias();//status
void analizarDeadlockDelSistema();//deadlock
void consolaPlanificador();

bool estaBloqueado(ESI_t* esi);
bool closureAVerSiSatisfaceDesbloqueo(ESI_t* esi);
bool closureParaDesbloquearClaveBloqueada(char* c);

#endif
