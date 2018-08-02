#ifndef CONSOLA_PLANIFICADOR_H
#define CONSOLA_PLANIFICADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include "ESIHandling/ESIHandling.h"//importar librearias
//#include "../Instancia/bibliotecaInstancia.h"


typedef enum{PAUSADO, PLANIFICANDO} est_Planif;

//funciones que pueden usar
t_queue* colaAsociada(char* clave);//busca en ListaColas la que se identifiqua con la clave y devuelve la cola
ESI_t* esiAVerSiDesbloqueo;

void pausarPlanificacion();
void continuarPlanificacion();

void bloquearProcesoESI(char* clave,int id);

void desbloquearProcesoESI(char* clave);

void listar(char* clave);

void finalizarProceso();//kill
void informacionDeInstancias();//status
void analizarDeadlockDelSistema();//deadlock
void consolaPlanificador();

ESI_t* buscarProcesoESI(int id);
ESI_t* buscarProcesoEnColas(t_queue* cola, int id);
bool estaBloqueado(ESI_t* esi);
bool closureAVerSiSatisfaceDesbloqueo(ESI_t* esi);

#endif
