#include "consolaPlanificador.h"

int isnum(char * str);

void consolaPlanificador(t_config* pConf){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE
	bloquearClaves(pConf);

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
		if(strcmp(p1,"desbloquear")== 0 && p2 != NULL && p3 == NULL)
		{
				printf("se intentara desbloquear la clave %s para el siguiente proceso bloqueado\n",p2);//la clave se guarda en p2
				desbloquearProcesoESI(p2);//si le pasas un cuarto parametro no lo toma y ejecuta igual
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
	cola_clave* c = buscarElementoDeLista(clave);

	if(c != NULL){ 
		if( p!= NULL ){
			pthread_mutex_lock(&m_listaColas);
			list_add(c->esisBloqueadosParaClave, p);
			pthread_mutex_unlock(&m_listaColas);
			printf("Correctamente bloqueado el ESI de id = %d para la clave %s.\n", id,clave);
		}
		else{
			printf("No hay proceso ESI con este ID\n");
		}
	}
	else{
		if(p!=NULL){ 
			c = new_cola_clave(clave, 0);
			pthread_mutex_lock(&m_listaColas);
			list_add(c->esisBloqueadosParaClave,p);
			list_add(ListaColas, c);
			pthread_mutex_unlock(&m_listaColas);
			printf("Correctamente bloqueado el ESI de id = %d para la clave %s.\n", id,clave);
		}
		else{
			printf("No hay proceso ESI con este ID\n");
		}
	}
}

ESI_t* buscarProcesoESI(int id){// busca en el sistema en la lista de listos y si el proceso esta ejecutando
		if(pESIEnEjecucion != NULL){ 
			if(pESIEnEjecucion->id == id){
				return pESIEnEjecucion;
			}
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
void desbloquearProcesoESI(char* clave){
	cola_clave* c = buscarElementoDeLista(clave);
	ESI_t* esi = NULL;
	if(!claveEstaBloqueada(clave)){ 
	if(c!=NULL){ 
		 esi = list_remove(c->esisBloqueadosParaClave,0);
		 if(esi!=NULL){
			pthread_mutex_lock(&m_listaColas);
			queue_push(c->cola, esi);
			pthread_mutex_unlock(&m_listaColas);
			printf("se ha desbloqueado el esi de id = %d para la clave %s\n",esi->id,clave);

			esiAVerSiDesbloqueo = esi;
			if(!estaBloqueadoPorOtraClave(esi)){ 
				if(!list_any_satisfy(ESIsListos->elements, (void *)&closureAVerSiSatisfaceDesbloqueo) && pESIEnEjecucion->id != esi->id){  
					pthread_mutex_lock(&m_colaListos);
					queue_push(ESIsListos,esi);
					pthread_mutex_unlock(&m_colaListos); 
					sem_post(&sem_cantESIsListos);
				}	 
			} 
		 }
		 else{
			printf("No quedan ESIS bloqueados para dicha clave\n");
		 }
		}
		else{
			printf("Esa clave no existe.\n");
		}
	}
	else{
		claveParaDesbloquearSiEstaBloqueada = clave;
		list_remove_by_condition(clavesBloqueadas, (void*)&closureParaDesbloquearClaveBloqueada);
		if(c!=NULL){ 
			pthread_mutex_lock(&m_listaColas);
			esi = queue_pop(c->cola);
			pthread_mutex_unlock(&m_listaColas);
			if(esi!=NULL){ 
				pthread_mutex_lock(&m_colaListos);
        		queue_push(ESIsListos, esi);
        		pthread_mutex_unlock(&m_colaListos);
			}
		}
	}
}

bool closureParaDesbloquearClaveBloqueada(char* c){
	return !strcmp(claveParaDesbloquearSiEstaBloqueada, c);
}

bool closureAVerSiSatisfaceDesbloqueo(ESI_t* esi){
		return esiAVerSiDesbloqueo->id == esi->id;
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
			printf("El proceso con id : %d espera la clave %s\n", p->id, clave);
			pElem = pElem->next;
		}
	}
	else
		printf("No hay procesos esperando esa clave\n");
}

void finalizarProceso(ESI_t* esi){ 
	ESI_t * pESI = quitarESIDeSuListaActual(esi->id);
	if(pESI)
		abortESI(pESI);
	else
		printf("No se ha encontrado un proceso con el id %d en el sistema\n",esi->id);
}
//kill

/*
void limpiarColaClave(int id){// ASE FALTA LIMPIAR primero donde esta la cola de claves si no quien lo elimina despues
	//se llamaba kill pero ahora puede llamrse limpiar y solo borro la ultima linea de eliminarESIDelSistema(ID)
	int i = 0;
	ESI_t *e ;
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	
	if (id == (pESIEnEjecucion -> id)){//Si es el que esta ejecutando
	printf("Que casualidad, intenta matar el proceso en ejecucion, [usted es incorregible]");
	}
	
	printf("Claves a Liberar: ");
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if(c-> idEsiUsandoClave == id){
			printf(" %s ",c->clave);
			i++;
			(c-> idEsiUsandoClave) = 0;//lo dejo en 0  pero no se que onda
		}
		else{
			e = buscarProcesoEnColas(c->cola,id);
			if(e != NULL){
				e = get_and_remove_ESI_by_ID(c->cola, id);
			}
		}
	p = p -> next ;
	}
	if(i==0){printf("ninguna");}
	eliminarESIDelSistema(id); 
}


*/


//STATUS

/*
void valor(char* clave){
	t_entrada_tabla * pEntry = findEnTablaEntrada(clave);
	if(pEntry != NULL){
		char*valor = obtenerValor(clave);
		printf("La clave: %s tiene valor: %s \n",clave,valor);
	}
	else {
	printf("No hay valor con esta clave");
	}
}
void buscarInstancia(char* clave){
	 inst_* i = instanciaDeLaClave(clave);
	 printf("Instancia actual %i en donde se encuentra la clave %s",i,clave);
	 while(i != NULL){
		i = i -> next;	
	}
	 printf("Proxima instancia vacia",i);
	NO PROBADO FALTA
	printf("FAlta");
} 
*/


void informacionDeInstancias(char * clave)
{
	//valor(clave);
	printf("Valor:\n");
	//buscarInstancia(clave);
	printf("Instancia:\n");
	printf("Instancia correcta actual:\n");
	printf("ESIs bloqueados:\n");
	listar(clave);
}

//DEADLOCK
bool esiPerteneceCola(t_queue* cola,int id){
	ESI_t* e = buscarProcesoEnColas(cola,id);
	return e != NULL;
}

void analizarDeadlockDelSistema(){
	int j=0;
	int id1;
	int id2;
	t_link_element* p = ListaColas -> head ;
	t_link_element* q;
	cola_clave* c1;
	cola_clave* c2;
	
	while (p != NULL){
		q = p -> next;
		c1 = (cola_clave*)(p -> data);
		id1 = c1 -> idEsiUsandoClave ;
		while(q != NULL){
			c2 = (cola_clave*)(q -> data);
			id2 = c2 -> idEsiUsandoClave ;
				if(esiPerteneceCola(c1->cola,id2) && esiPerteneceCola(c2->cola,id1) && id1 != id2){
					printf("Deadblock de esis %d y %d ",id1,id2);//poner un barra invertida n al final
					j++;
				}
			q = q -> next ;
		}
	 p = p -> next	;
	}
	if(j == 0){ printf("No hay clave(recurso) que un ESI use y que otro ESI espere este recurso");}

}
