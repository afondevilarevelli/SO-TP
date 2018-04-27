#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>//importar librearias

void pausarPlanificaion(){}
void continuarPlanificaion(){}
void bloquearProcesoESI(){}//no se si se le pasa un parametro
void desbloquearProcesoESI(){}//desbloquear
void listarProcesos(){}//listar
void finalizarProceso(){}//kill
void informacionDeInstancias(){}//status
void analizarDeadlockDelSistema(){}//deadlock

int main(){
	char* linea;
	char espaBlan[4]=" \n\t";
	linea = readline(">"); //asigna toda la cadena escrita con puntero, linea

	while(*linea != '0'){//si escribo 0  y preciono enter se termina la ejecucion   
// char* p1 = malloc(10);  luego abajo dentro del while free(p1);
	char* p1 = strtok(linea,espaBlan);    // toquen apunta al primer parametro
	char* p2 = strtok(NULL,espaBlan);	 // toquen apunta al segundo parametro
	char* p3 = strtok(NULL,espaBlan);   // toquen apunta al tercer parametro

	if(strcmp(p1,"pausar")== 0){// && p2 == NULL no importa el segundo parametro
			printf("tiene pausa. Esta bien escrita: \n");
				pausarPlanificaion();
		}
	if(strcmp(p1,"continuar")== 0){//&& p2 == NULL no importa el segundo parametro
			printf("tiene continuar. Esta bien escrita \n");
				continuarPlanificaion();
		}
	if(strcmp(p1,"bloquear")== 0 && p2 != NULL && p3 != NULL){
			printf("tiene bloquear. Esta bien escrita \n");//la clave se guarda en p2 y el ID en p3
					bloquearProcesoESI();//si le pasas un cuarto parametro no lo toma y ejecuta igual
		}
	if(strcmp(p1,"listar")== 0 && p2 != NULL && p3 == NULL){
			printf("tiene listar. Esta bien escrito \n");//el recurso esta guardado en p2
		}
	if(strcmp(p1,"kill")== 0 && p2 != NULL && p3 == NULL){
		printf("tiene kill. Esta bien escrito  \n");// El ID se guarda en p2
		finalizarProceso();
	}
	if(strcmp(p1,"status")== 0 && p2 != NULL && p3 == NULL){
			printf("tiene status. Esta bien escrito  \n");// El clave se guarda en p2
			informacionDeInstancias();
		}
	if(strcmp(p1,"deadlock")== 0 && p2 == NULL ){
				printf("tiene deadlock. Esta bien escrito  \n");
				analizarDeadlockDelSistema();
			}
	free(linea);//si antes de liberarlos los dejo en NULL a p1,p2,p3 o se liberan al final
	linea = readline(">");
	}//restringo los parametros asi se quedan en NULL, y si se llenan se pierde ?
return 0;
}
