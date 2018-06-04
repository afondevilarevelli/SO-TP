//2) bloquear el proceso ESI por consola
//no olvidar poner la libreria <commons/collection/queue.h>  de las colas
// yo puse al tipo de dato esi o proceso esi -> le puse* pro_ESI
typedef enum{LISTOS,EJECUTANDO,BLOQUEADO} t_cola;

//creo una structura para tipo de dato ESI
typedef struct {
	int id_esi;
	t_cola colaProviniente;
}pro_ESI;

t_queue* colaAsociada(char clave){
	t_queue *p ;
	//proceso de hallar la cola asociada a esa clave(falta desarrollar) que lo crea planificador;
	return p;//la cola asociada a esa clave;
}

rtdoEject_t bloquear(int id,int clave){
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

//3)desbloquear
rtdoEject_t sacarProcesoEnLista(clave){
	t_queue*c = colaAsociada(clave);
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
//4)
void listar(clave){//recurso == clave
		t_queue* c = colaAsociada(clave);
		pro_ESI* p;
		
		t_link_element* pElem = (c -> elemento) -> head ;
	while(pElem != NULL){// si la cola no esta vacia
		
		p = (pro_ESI*)(pElem->data);//use el mecanismo de Antonio de Las Carreras todos los creditos a EL
		printf("El proceso con id : %d \n", p->id_esi);
		pElem = pElem->next;
	}
}
