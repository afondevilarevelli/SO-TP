2) //bloquear el proceso ESI por consola
//no olvidar poner la libreria <commons/collection/queue.h>  de las colas
// yo puse al tipo de dato esi o proceso esi ,,,,,le puse* pro_ESI
typedef enum{LISTOS,EJECUTANDO,BLOQUEADO} t_cola;

//PROBLEMA COMO vuelvo a recordar en que cola estaba 
int tipoColaProveniente = 0 ;

rtdoEject_t bloquear(ID,clave){
	pro_ESI* p =	buscarProcesoESI(ID);//no se que tipo de dato es ID . Yo le puse que clave es un puntero a una cola pero eso se puede cambiar con un identificador asia esa clave
	t_queue *clave = queue_create();//crea una nueva cola llamada ,bueno el puntero se llama clave

	if( p!= NULL ){
		 queue_push(clave, (pro_ESI *)p);//Agrega un elemento al final de la cola, fijarse si esta bien escrito
		 return SUCESS;
	}
	printf("No hay procesoESI con este ID");	
	return FAILURE;
}



pro_ESI* buscarProcesoESI(ID){
	pro_ESI* p ;
	// Puede ser,  como solo son 3 colas bloqueados, ejecutando y bloqueados y apaarte me dicen que no esta en las bloqueadas
	p = buscarProcesoEnLista(ESIsListos,Proceso);//ColaListos de coordinador.c
	
		if(p != NULL){//osea lo encontro en la cola de Listos
		tipoColaProveniente = LISTOS ;
			return p;
		}
		else{//no estaba en la ColaListos
		
			// no hay cola de ejecutados cambiar a si if(se esta ejecutando y && not esatomico(proceso))
			p = buscarProcesoEnLista(ListaDeEjecutados,proceso)//ListaDeEjecutados de coordinador.c
			if(p != NULL){//falta crear una lista de ejecutados o no se donde estan
			tipoColaProveniente = EJECUTANDO ;
			return p;
			}
			//	else{//osea no esta en ninguna de las dos colas y quedo en NULL	//	}
		}		
		printf("No esta en ni en la cola de ejecutados ni en la de listos");
		return p;
}
pro_ESI* buscarProcesoEnLista(t_queue cola,pro_ESI* proceso){
	pro_ESI* p ;//por ai que falta un malloc y luego un free
	t_link_element* pElem = (cola -> elemento) -> head ;
	while(pElem != NULL){
		
		p = (pro_ESI*)(pElem->data);//use el mecanismo de antonio todos los creditos a EL
		if(p == proceso){// no se si se puede hacer directamente esto como no conoxco la estructura del proceso, lo dejo en pseudocodigo
		return p;
		}
		pElem = pElem->next;
	}
	return NULL;// SI NO ESTA EN LA cola BOTA NULL la funcion
}

3)//desbloquear
rtdoEject_t sacarProcesoEnLista(clave){
	t_queue *clave ;//falta encontrar la cola que se identifica con el parametro clave
	
	pro_ESI* p=	buscarProcesoEnLista(clave,Proceso);
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
		switch (tipoColaProveniente)//como guardo el estado de la cola
		{
			case LISTOS:	mandarAlFinalCola(p,ColaListos); break;
			
			case EJECUTANDO:	mandarAlFinalCola(p,ListaDeEjecutados); break;
		}
		return 	SUCESS;
	}
	printf("Este proceso no fue bloquedo por consola");
	return FAILURE;
}
4)//listar por recurso		(*) yo interprete que saca de la colaDe bloqueados, los que estan esperando ese resurso especifico
listar <recurso>: Lista los procesos encolados esperando al recurso.
no entiendo que piden? que lo liste que (*) , si es asi donde en otra cola .  Ademas lo debo mostrar?


void listar(recurso){
		t_cola claveRecurso = new Cola();
		pro_ESI* p= ESIsBloqueados;// esta en planificador.c es la cola de bloqueados creo pero no estoy seguro solo lo puse en esta linea
		//por ai un if de recurso no valido ? y camabiamos el tipo void POR rtdoEject_t y si entra return FAILURES;
		while(p!= NULL){
			if(procesoEsperaRecurso(*p,recurso)){
			mandarAlFinalCola(*p,claveRecurso);
			}
			*(p ++) ;
		}
		// return SUCESS;
}

bool procesoEsperaRecurso(pro_ESI* proceso,t_recurso recurso){
	
		if(estaEnLaLista(ListaDeBloqueados,proceso){
		return esperaRecurso(proceso,recurso)	;
		}
return false;//no esta en la lista de bloqueados
}

bool esperaRecurso(proceso,recurso){
	//algoritmo   y se puede borrar la funcion y directamente ponerla arriba si no es tedioso el procesidimiento
}	
