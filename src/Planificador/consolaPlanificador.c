#include "consolaPlanificador.h"

int isnum(char * str);

void consolaPlanificador(){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE

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
			printf("La planificacion ahora se encuentra pausada\n");
			pausarPlanificacion();
		}
		else
		if(strcmp(p1,"continuar")== 0)
		{//&& p2 == NULL no importa el segundo parametro
			printf("La planificacion ahora se encuentra despausada\n");
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
		if(strcmp(p1,"desbloquear")== 0 && p2 != NULL && p3 != NULL)
		{
			if(!isnum(p3))
				printf("El segundo parametro debe ser un numero.\n");
			else
			{
				int id = atoi(p3);
				printf("se intentara desbloquear la clave %s para el proceso %s\n",p2, p3);//la clave se guarda en p2
				desbloquearProcesoESI(p2, id);//si le pasas un cuarto parametro no lo toma y ejecuta igual
			}
		}
		else
		if(strcmp(p1,"listar")== 0 && p2 != NULL )
		{
			printf("lista de procesos esperando el recurso con clave %s \n", p2);//el recurso esta guardado en p2
			listar(p2);
		}
		else
		if(strcmp(p1,"kill")== 0 && p2 != NULL )
		{
			if( isnum(p2) )
			{
				int id = atoi(p2);
				ESI_t* elESI = buscarProcesoESI(id);
				finalizarProceso(elESI);
				printf("se ha matado al proceso con id = %d \n", id);// El ID se guarda en p2
			}
			else
				printf("Error. La sintaxis del comando es:\n\tkill <process-id>\n");
		}
		else
		if(strcmp(p1,"status")== 0 && p2 != NULL)
		{
			printf("estado de la clave %s: \n", p2);// El clave se guarda en p2
			informacionDeInstancias(p2);
		}
		else
		if(strcmp(p1,"deadlock")== 0 )
		{
			printf("deadlocks del sistema: \n");
			analizarDeadlockDelSistema();
		}
		else
		{
			debeContinuar = strcmp(linea, "salir");
			if(debeContinuar)printf("comando no reconocido\n");
		}

	}while(debeContinuar);//restringo los parametros asi se quedan en NULL, y si se llenan se pierde ?

	free(linea);

	exit(0);

	return;
}

int isnum(char * str)
{
	int i = 0, result;

	for( i = 0; str[i] != '\0'; i++)
		if( !isdigit(str[i]) )
			return 0; //FALSE

	return 1; //TRUE
}

void pausarPlanificacion(){
	pthread_mutex_trylock(&m_puedeEjecutar);
}
void continuarPlanificacion(){
	pthread_mutex_unlock(&m_puedeEjecutar);
}

t_queue* colaAsociada(char* clave){//busca de mi ListaColas la que se identifiqua con la clave
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if (!strcmp(c -> clave, clave)){
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
	else
	{
	printf("No hay procesoESI con este ID o ya esta bloqueado\n");
	}
}

ESI_t* buscarProcesoESI(int id){// busca en el sistema en la lista de listos y si el proceso esta ejecutando
		if(pESIEnEjecucion->id == id){
			return pESIEnEjecucion;
		}
		ESI_t* p = buscarProcesoEnColas(ESIsListos,id);
		if(p != NULL ){
			return p;
		}
		else{
			p = buscarProcesoEnColas(ESIsBloqueados, id);
			if(p!=NULL){
				return p;
			}
		}
		printf("No esta ejecutando y tampoco esta en la cola de listos ni de bloqueados\n");
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

bool estaBloqueado(ESI_t* esi){
	ESI_t* pEsi = buscarProcesoEnColas(ESIsBloqueados, esi->id);
	return pEsi != NULL;
}

//3)desbloquear
void desbloquearProcesoESI(char* clave, int id){
	t_queue*c = colaAsociada(clave);
	ESI_t* p = NULL;
	if(c!= NULL ){  pthread_mutex_lock(&m_listaColas);
					p = get_and_remove_ESI_by_ID(c->elements, id);
				    pthread_mutex_unlock(&m_listaColas); }
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
			pthread_mutex_lock(&m_colaListos);
			queue_push(ESIsListos,p);
			pthread_mutex_unlock(&m_colaListos);
	}else
	printf("Esa clave no existe.\n");
}
//4)
void listar(char* clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		ESI_t* p;

		t_link_element* pElem = c?(c -> elements) -> head:NULL ;
	if(pElem)
	{
		while(pElem != NULL){// si la cola no esta vacia

			p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
			printf("El proceso con id : %d \n", p->id);
			pElem = pElem->next;
		}
	}
	else
		printf("No hay procesos esperando esa clave\n");
}

void finalizarProceso(ESI_t* esi){  // ACÁ DEBERÍA TAMBIEN ELIMINARLO DE LA COLA DE UNA CLAVE, SI ES QUE ESTÁ BLOQUEADO POR UNA CLAVE!
	ESI_t * pESI = quitarESIDeSuListaActual(esi->id);
	if(pESI)
		abortESI(pESI);
	else
		printf("No se ha encontrado un proceso con el id %d en el sistema\n",esi->id);
}//kill
void informacionDeInstancias(char * clave)
{
	printf("Valor:\n");
	printf("Instancia:\n");
	printf("Instancia correcta actual:\n");
	printf("ESIs bloqueados:\n");
	listar(clave);
}//status
void analizarDeadlockDelSistema(){}

char* claveESIenEjecucion(){
	ESI_t* e = pESIEnEjecucion;
	return "La clave que usa dicho esi";
}
//esiEjecucion() devuelve el ESI_t* que esta ejecutando
void deadlock(){
	t_queue* c;
	ESI_t* e;
	char* clave = claveESIenEjecucion();
	c = colaAsociada(clave);
	if(queue_size(c)> 0){
		e = queue_peek(c);//queue_peek devuelve el primer elemento sin extraerlo
		printf("El ESI en ejecucion con ID = %d usa la clave:%s ,que esta esperando el ESI ID = %d",pESIEnEjecucion->id,clave,e->id);
	}
	else{
	printf("No hay clave(recurso) que un ESI use y que otro ESI espere este recurso");
	}
}
