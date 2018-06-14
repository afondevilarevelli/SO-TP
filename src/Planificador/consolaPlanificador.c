#include "consolaPlanificador.h"

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

		if( p1 == NULL )
			continue;
		else
		if(strcmp(p1,"pausar")== 0)
		{// && p2 == NULL no importa el segundo parametro
			printf("se puso en pausa la planificacion \n");
			pausarPlanificacion();
		}
		else
		if(strcmp(p1,"continuar")== 0)
		{//&& p2 == NULL no importa el segundo parametro
			printf("se continua la planificacion \n");
			continuarPlanificacion();
		}
		else
		if(strcmp(p1,"bloquear")== 0 && p2 != NULL && p3 != NULL)
		{
			int id = atoi(p3);	//convertir p3 a int es el id
			printf("intentando bloquear la clave %s para el proceso con id = %d... \n", p2, id );//la clave se guarda en p2 y el ID en p3
			bloquearProcesoESI(p2,id);
		}
		else
		if(strcmp(p1,"desbloquear")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("se intentara desbloquear la clave %s para el primer proceso en la cola de espera de dicha clave \n",p2);//la clave se guarda en p2
			desbloquearProcesoESI(p2);//si le pasas un cuarto parametro no lo toma y ejecuta igual
		}
		else
		if(strcmp(p1,"listar")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("lista de procesos esperando el recurso con clave %s \n", p2);//el recurso esta guardado en p2
			listar(p2);
		}
		else
		if(strcmp(p1,"kill")== 0 && p2 != NULL && p3 == NULL)
		{
			int id = atoi(p2);
			printf("se ha matado al proceso con id = %d \n", id);// El ID se guarda en p2
			finalizarProceso();
		}
		else
		if(strcmp(p1,"status")== 0 && p2 != NULL && p3 == NULL)
		{
			printf("estado de la clave %s: \n", p2);// El clave se guarda en p2
			informacionDeInstancias();
		}
		else
		if(strcmp(p1,"deadlock")== 0 && p2 == NULL )
		{
			printf("deadlocks del sistema: \n");
			analizarDeadlockDelSistema();
		}

	}while(strcmp(linea, "salir"));//restringo los parametros asi se quedan en NULL, y si se llenan se pierde ?

	free(linea);

	exit(0);

	return;
}

void pausarPlanificacion(){
	pthread_mutex_lock(&m_puedeEjecutar);
}
void continuarPlanificacion(){
	pthread_mutex_unlock(&m_puedeEjecutar);
}

t_queue* colaAsociada(char* clave){//busca de mi ListaColas la que se identifiqua con la clave
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if (strcmp(c -> clave, clave)){
			return c -> cola;//esta bien escrito
		}
	p = p -> next ;
	}
	return NULL ;
}
//2) bloquear el proceso ESI por consola
void bloquearProcesoESI(char* clave,int id){
	ESI_t* p = buscarProcesoESI(id);
	t_queue *c = colaAsociada(clave);

	if( p!= NULL ){
		p->tiempoEsperandoCPU = 0;
		 queue_push(c, (ESI_t *)p);//Agrega un elemento al  de la cola
		 printf("Correctamente bloqueado el ESI.\n");
	}
	else{
	printf("No hay procesoESI con este ID o ya esta bloqueado\n");
	}
}

ESI_t* buscarProcesoESI(int id){// busca en el sistema en la lista de listos y si el proceso esta ejecutando
	ESI_t* p = buscarProcesoEnColas(ESIsListos,id);//ColaListos : variable de coordinador.c

		if(p != NULL /*|| id == (procesoEjecutando()->id)*/){//procesoEjecutando();funcion del  planificador.c que me diga cual esi esta en ejecutando
			return p;
		}
		printf("No esta ejecutando y tampoco esta en la cola de listos\n");
		return p;
}

ESI_t* buscarProcesoEnColas(t_queue* cola,int id){
	ESI_t* p ;
	t_link_element* pElem = (cola -> elements) -> head ; //
	while(pElem != NULL){

		p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		if(id == p->id){
		return p;
		}
		pElem = pElem->next;
	}
	return NULL;// SI NO ESTA EN LA cola BOTA NULL la funcion
}

//3)desbloquear
void desbloquearProcesoESI(char* clave){
	t_queue*c = colaAsociada(clave);
	ESI_t* p = NULL;
	if(c!= NULL ) p = queue_pop(c);//	(t_queue*)			Eliminar el primer elemento
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
			queue_push(ESIsListos,p);
	}else
	printf("Esa clave no existe.\n");
}
//4)
void listar(char* clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		ESI_t* p;

		t_link_element* pElem = c?(c -> elements) -> head:NULL ;
	while(pElem != NULL){// si la cola no esta vacia

		p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		printf("El proceso con id : %d \n", p->id);
		pElem = pElem->next;
	}
}



void finalizarProceso(int id){
	ESI_t * pESI = quitarESIDeSuListaActual(id);
	abortESI(pESI);
}//kill
void informacionDeInstancias(){}//status
void analizarDeadlockDelSistema(){}
