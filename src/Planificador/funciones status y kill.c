kill(int id){
char* clave;
ESI_t *e ;
	if (id == pESIEnEjecucion -> id){//Si esta ejecutando
		clave = claveESIenEjecucion();
		//hay que liberar esta clave
		free(ESI_t*);//liberar recursos(id /*del esi*/)
		}
	if(buscarProcesoEnColas(ESIsListos,id)!=NULL){	//si esta en la cola de listos
		e = get_and_remove_ESI_by_ID(ESIsListos->elements, id);
	}
	if(buscarProcesoEnColas(ESIsBloqueados, id)!=NULL){//si esta en la cola de bloqueados
		e = get_and_remove_ESI_by_ID(ESIsBloqueados->elements, id);
	}
	
}

void status(char* clave){
	valor(clave);
	buscarInstancia(clave);//posicion de memoria en donde se encuentra guardado esa clave
	listar(clave);//muestra los esis que esperan esa clave e imprime "no hay esis esperando" si no hay
}

void valor(char* clave){
	t_entrada_tabla * pEntry = findEnTablaEntrada(clave);
	if(pEntry != NULL){
		char*valor = obtenerValor(clave);// #include "bibliotecaInstancia.h"
		printf("La clave: %s tiene valor: %s",clave,valor);
	}
	else {
	printf("No hay valor con esta clave");
	}
}
void buscarInstancia(char* clave){
	/* inst_* i = instanciaDeLaClave(clave);
	 printf("Instancia actual %i en donde se encuentra la clave %s",i,clave);
	 while(i != NULL){
		i = i -> next;	
	}
	 printf("Proxima instancia vacia",i);
	*/
}

char* claveESIenEjecucion(){
	int id = pESIEnEjecucion -> id;
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if(c-> idEsiUsandoClave == id){
			return c->clave ;
		}
	p = p -> next ;
	}
	return "NO" ;//Si no hay retorna NO
}


void deadlock(){
	t_queue* c;
	ESI_t* e;
	char* clave = claveESIenEjecucion();
	c = colaAsociada(clave);
	if(queue_size(c)> 0){
		e = queue_peek(c);//queue_peek devuelve el primer elemento sin extraerlo
		printf("El ESI en ejecucion con ID = %d usa la clave:%s ,que esta esperando el ESI ID = %d",esiEjecucion()->id,clave,e ->id);
	}
	else{
	printf("No hay clave(recurso) que un ESI use y que otro ESI espere este recurso");
	}
}
