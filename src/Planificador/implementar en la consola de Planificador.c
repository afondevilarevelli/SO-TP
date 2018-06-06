#include "ESIHandling/ESIHandling.h"

typedef struct {
	t_queue* cola ;
	char** clave;
}cola_clave;

list* ListaColas = list_create();

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
void bloquear(int id,char* clave){
	ESI_t* p = buscarProcesoESI(id);
	t_queue *c = colaAsociada(clave);

	if( p!= NULL ){
		 queue_push(c, (ESI_t *)p);//Agrega un elemento al  de la cola
		 printf("Correctamente bloqueado el ESI.\n");
	}
	printf("No hay procesoESI con este ID o ya esta bloqueado");	
}

ESI_t* buscarProcesoESI(int id){// busca en el sistema en la lista de listos y si el proceso esta ejecutando 
	ESI_t* p ;
	p = buscarProcesoEnColas(ESIsListos,id);//ColaListos : variable de coordinador.c
	
		if(p != NULL /*|| id == (procesoEjecutando()->id)*/){//procesoEjecutando();funcion del  planificador.c que me diga cual esi esta en ejecutando
			return p;
		}	
		printf("No esta ejecutando y tampoco esta en la cola de listos");
		return p;
}
ESI_t* buscarProcesoEnColas(t_queue* cola,int id){
	ESI_t* p ;
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
void desbloquear(char* clave){
	t_queue*c = colaAsociada(clave);
	ESI_t* p;
	p = queue_pop(c);//	(t_queue*)			Eliminar el primer elemento
	
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
			queue_push(ESIsListos,p); break;
	}
	printf("Esa clave no existe.\n");
}
//4)
void listar(char* clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		ESI_t* p;
		
		t_link_element* pElem = (c -> elemento) -> head ;
	while(pElem != NULL){// si la cola no esta vacia
		
		p = (ESI_t*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		printf("El proceso con id : %d \n", p->id);
		pElem = pElem->next;
	}
}
