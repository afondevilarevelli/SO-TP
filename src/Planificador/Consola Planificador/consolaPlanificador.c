/* strcpy(str1,str2); str1 =str2
 * strcat(str1,str2); str1 =str1 + str2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pausarPlanificaion(){}
void continuarPlanificaion(){}
void bloquearProcesoESI(){}//no se si se le pasa un parametro
void desbloquearProcesoESI(){}//desbloquear
void listarProcesos(){}//listar
void finalizarProceso(){}//kill
void informacionDeInstancias(){}//status
void analizarDeadlockDelSistema(){}//deadlock

int main(){
	char cad[50];//si pongo "palabra" y "palabra " igual deja a los dos parametros siguientes en null
	char espaBlan[4]=" \n\t";


	fgets(cad,50,stdin);
	while(cad[0]!= '0'){//si escribo 0  y preciono enter se termina la ejecucion
	char* p1 = strtok(cad,espaBlan);    // toquen apunta al primer parametro
	char* p2 = strtok(NULL,espaBlan);	// toquen apunta al segundo parametro
	char* p3 = strtok(NULL,espaBlan);   // toquen apunta al tercer parametro
	//la cadena cad se detruyo y solo quedo el primer token en cad,

	if(strcmp(p1,"pausar")== 0 && p2 == NULL){
			printf("tiene pausa. Esta bien escrita: \n");
				pausarPlanificaion();
		}
	if(strcmp(p1,"continuar")== 0 && p2 == NULL){
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
				// En este espacio puedo liberar la memoria
	fgets(cad,50,stdin);
	}
return 0;
}
