#include "consolaPlanificador.h"

int puedeEjecutarPlanif = 1;

void consolaPlanificador(){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";


	do{

		free(linea);//si antes de liberarlos los dejo en NULL a p1,p2,p3 o se liberan al final
		linea = readline(">");//si escribo 0  y preciono enter se termina la ejecucion
	// char* p1 = malloc(10);  luego abajo dentro del while free(p1);
		char* p1 = strtok(linea,espaBlan);    // toquen apunta al primer parametro
		char* p2 = strtok(NULL,espaBlan);	 // toquen apunta al segundo parametro
		char* p3 = strtok(NULL,espaBlan);   // toquen apunta al tercer parametro

		if(strcmp(p1,"pausar")== 0)
		{// && p2 == NULL no importa el segundo parametro
			printf("tiene pausa. Esta bien escrita: \n");
			pausarPlanificacion();
		}

		if(strcmp(p1,"continuar")== 0)
		{//&& p2 == NULL no importa el segundo parametro
			printf("tiene continuar. Esta bien escrita \n");
			continuarPlanificacion();
		}

		if(strcmp(p1,"bloquear")== 0 && p2 != NULL && p3 != NULL)
		{
			printf("tiene bloquear. Esta bien escrita \n");//la clave se guarda en p2 y el ID en p3
			int id = atoi(p3);	//convertir p3 a int es el id
			bloquearProcesoESI(p2,id);
		}
		
		if(strcmp(p1,"desbloquear")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("tiene desbloquear. Esta bien escrita \n");//la clave se guarda en p2
			desbloquearProcesoESI(p2);//si le pasas un cuarto parametro no lo toma y ejecuta igual
		}

		if(strcmp(p1,"listar")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("tiene listar. Esta bien escrito \n");//el recurso esta guardado en p2
			listarProcesos(p2);
		}

		if(strcmp(p1,"kill")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("tiene kill. Esta bien escrito  \n");// El ID se guarda en p2
			finalizarProceso();
		}

		if(strcmp(p1,"status")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("tiene status. Esta bien escrito  \n");// El clave se guarda en p2
			informacionDeInstancias();
		}

		if(strcmp(p1,"deadlock")== 0 && p2 == NULL )
		{
			printf("tiene deadlock. Esta bien escrito  \n");
			analizarDeadlockDelSistema();
		}


	}while(*linea != '0');//restringo los parametros asi se quedan en NULL, y si se llenan se pierde ?

	free(linea);



	return;
}

void pausarPlanificacion(){
	puedeEjecutarPlanif = 0;
}
void continuarPlanificacion(){
	puedeEjecutarPlanif = 1;
}

t_queue* colaAsociada(char *clave){
	t_queue *p ;
	//proceso de hallar la cola asociada a esa clave(falta desarrollar) que lo crea planificador;
	return p;//la cola asociada a esa clave;
}

rtdoEject_t bloquearProcesoESI(int id,char* clave){
	pro_ESI* p = buscarProcesoESI(id);
	t_queue *c = colaAsociada(clave);

	if( p!= NULL ){
		 queue_push(c, (pro_ESI *)p);//Agrega un elemento al  de la cola
		 return SUCESS;final
	}
	printf("No hay procesoESI con este ID o ya esta bloqueado");	
	return FAILURE;
}

pro_ESI* buscarProcesoESI(int id){// ID int
	pro_ESI* p ;
	p = buscarProcesoEnColas(ESIsListos,id);//ColaListos : variable de coordinador.c
	
		if(p != NULL){//lo encontro en la cola de Listos
		p ->colaProviniente = LISTOS ;
			return p;
		}
		else{//no estaba en la ColaListos 
		//pESIEnEjecucionv : variable de  planificador.c
			if(pESIEnEjecucion->id_esi == id){ // necesito analizar si es atomico?
				p ->colaProviniente = EJECUTANDO;
				return p;
			}
		}		
		printf("No esta ejecutando y tampoco esta en la cola de listos");
		return p;
}

pro_ESI* buscarProcesoEnColas(t_queue* cola,int id){
	pro_ESI* p ;//por ai que falta un malloc y luego un free?
	t_link_element* pElem = (cola -> elemento) -> head ; //
	while(pElem != NULL){
		
		p = (pro_ESI*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		if(id == p->id_esi){
		return p;
		}
		pElem = pElem->next;
	}
	return NULL;// SI NO ESTA EN LA cola BOTA NULL la funcion
}

rtdoEject_t desbloquearProcesoESI(char* clave){
	t_queue*c = colaAsociada(clave);//falta ver
	pro_ESI* p;
	p = queue_peek(c);//devuelve el primer elemento
	queue_pop(c);//Eliminar el primer elemento
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
		switch (p->colaProviniente)
		{ 
			case LISTOS:	queue_push(ESIsListos,p); break;
			
			case EJECUTANDO:	p -> colaProviniente = LISTOS; queue_push(ESIsListos,p);  break;//lo mando a la cola de listos
		}
		return 	SUCESS;
	}
	printf("Esa clave no existe ");
	return FAILURE;
}

void listarProcesos(char* clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		pro_ESI* p;
		
		t_link_element* pElem = (c -> elemento) -> head ;
	while(pElem != NULL){// si la cola no esta vacia
		
		p = (pro_ESI*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		printf("El proceso con id : %d \n", p->id_esi);
		pElem = pElem->next;
	}
}
void finalizarProceso(){}//kill
void informacionDeInstancias(){}//status
void analizarDeadlockDelSistema(){}
int puedeEjecutar(){
	return puedeEjecutarPlanif;
}
