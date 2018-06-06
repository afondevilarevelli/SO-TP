#include "ESIHandling/ESIHandling.h"

t_queue* colaAsociada(char clave){
	t_link_element* p = ListaColas -> head ;
	cola_clave* c;
	while (p != NULL){
		c = (cola_clave*)(p -> data);
		if ( strcmp(c -> clave, clave == 0) ){
			return c -> cola;
		}
	p = p -> next ;
	}
	return NULL ;
}
//2) bloquear el proceso ESI por consola
rtdoEject_t bloquear(int id,char clave){
	ESI_t* p = buscarProcesoESI(id);
	t_queue *c = colaAsociada(clave);

	if( p!= NULL ){
		 queue_push(c, (ESI_t *)p);//Agrega un elemento al  de la cola
		 return SUCESS;final
	}
	printf("No hay procesoESI con este ID o ya esta bloqueado");	
	return FAILURE;
}

ESI_t* buscarProcesoESI(int id){// ID int
	ESI_t* p ;
	p = buscarProcesoEnColas(ESIsListos,id);//ColaListos : variable de coordinador.c
	
		if(p != NULL){//lo encontro en la cola de Listos
		p ->colaProviniente = LISTOS ;
			return p;
		}
		else{//no estaba en la ColaListos 
		//pESIEnEjecucionv : variable de  planificador.c
			if(pESIEnEjecucion->id == id){ // necesito analizar si es atomico?
				p ->colaProviniente = EJECUTANDO;
				return p;
			}
		}		
		printf("No esta ejecutando y tampoco esta en la cola de listos");
		return p;
}
ESI_t* buscarProcesoEnColas(t_queue* cola,int id){
	ESI_t* p ;//por ai que falta un malloc y luego un free?
	t_link_element* pElem = (cola -> elemento) -> head ; //
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
rtdoEject_t sacarProcesoEnLista(char clave){
	t_queue*c = colaAsociada(clave);
	ESI_t* p;
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
//4)
void listar(char clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		ESI_t* p;
		
		t_link_element* pElem = (c -> elemento) -> head ;
	while(pElem != NULL){// si la cola no esta vacia
		
		p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		printf("El proceso con id : %d \n", p->id);
		pElem = pElem->next;
	}
}
