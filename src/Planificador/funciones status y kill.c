
//recursos#(claves)   ListaColas // si esta bloqueada

kill(int id){
list_element *lis =	ListaColas;
ESI_t *e ;
	if (id == procesoEjecutando()-> id){//Si esta ejecutando
		free(ESI_t*);//liberar recursos(id /*del esi*/)
		procesoEjecutando()=NULL;
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
printf("comoo es que tiene valor una clave si es un char*");
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
