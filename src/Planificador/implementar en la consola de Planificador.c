En los voids meter esto

1)// cambiar el estado del planificador en (planificador.c) a no esperar ESIs y viceversa 
en planificador.c en la funcion 

void planificarEjecucionESI(void)
{
	while(puedeEjecutar())  // Aqui cambiar el valor a false de puedeEjecutar y viceversa 
	{

// Como -> puede ser con un socket  o hacceder a la variable o funcion booleana por biblioteca
//Esto dice el pdf no se que pensar estuve viendo la funcion syscall() pero no logro concentar la idea .Esto se puede lograr ejecutando una sycall bloqueante que espere la entrada de un humano

2) //bloquear el proceso ESI por consola

typedef enum{LISTOS,EJECUTANDO,BLOQUEADO} t_Lista;
int listaProveniente = 0 ;

rtdoEject_t bloquear(ID,clave){
	pro_ESI p =	buscarProcesoESI(ID);
	if( p!= NULL ){
		 mandaCola(p,clave);
		 return SUCESS;
	}
	printf("No hay procesoESI con este ID");	
	return FAILURE;
}

void mandaCola(Proceso,clave){
	t_cola clave = new Cola();
	ponerEnLaCola(clave,Proceso);
}

pro_ESI buscarProcesoESI(ID){
	pro_ESI p ;
	// Puede ser,  como solo son 3 listas bloqueados, ejecutando y bloqueados y apaarte me dicen que no esta en las bloqueadas
	pro_ESI p = buscarProcesoEnLista(ListaDeListos,Proceso)
	
	//aca hay que meterle un if(esatomico(proceso))  y la funcion "esatomico" que detecta si es o no >> bool
		if(p != NULL){//osea lo encontro en la lista de Listos
		listaProveniente = LISTOS ;
			return p;
		}
		else{//no estaba en ala listadeListos
			p = buscarProcesoEnLista(ListaDeEjecutados,Proceso)
			if(p != NULL){//osea lo encontro en la cola de ejecutados
			listaProveniente = EJECUTANDO ;
			return p;
			}
			//	else{//osea no esta en ninguna de las dos colas y quedo en NULL	//	}
		}		
		printf("No esta en ni en la cola de ejecutados ni en la de listos");
		return p;
}
pro_ESI buscarProcesoEnLista(Lista,Proceso){//no se si era lista o cola
	pro_ESI p ;
	//while o algo
		if(estaEnLaLista(Lista,Proceso)){
			//El algoritmo de buscar un elemento de una lista
		return p;
		}
	return 	NULL;/////// SI NO ESTA EN LA LISTA BOTA NULL la funcion
}
boll estaEnLaLista(Lista,Proceso){
//El algoritmo de buscar un elemento de una lista
return true;
}

3)//desbloquear
rtdoEject_t sacarProcesoEnLista(clave){
	t_cola lista =  cola(clave);
	
	pro_ESI p=	buscarProcesoEnLista(lista,Proceso);
	if(p!= NULL ){//p es el proceso buscado, ahora hay que mandarlo al final de la cola de donde se saco
		switch (listaProveniente)
		{
			case LISTOS:	mandarAlFinalCola(p,ListaDeListos); break;
			
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
		pro_ESI *p= ESIsBloqueados;// esta en planificador.c es la cola de bloqueados creo pero no estoy seguro solo lo puse en esta linea
		//por ai un if de recurso no valido ? y camabiamos el tipo void POR rtdoEject_t y si entra return FAILURES;
		while(p!= NULL){
			if(procesoEsperaRecurso(*p,recurso)){
			mandarAlFinalCola(*p,claveRecurso);
			}
			*(p ++) ;
		}
		// return SUCESS;
}

bool procesoEsperaRecurso(pro_ESI proceso,t_recurso recurso){
	
		if(estaEnLaLista(ListaDeBloqueados,proceso){
		return esperaRecurso(proceso,recurso)	;
		}
return false;//no esta en la lista de bloqueados
}

bool esperaRecurso(proceso,recurso){
	//algoritmo   y se puede borrar la funcion y directamente ponerla arriba si no es tedioso el procesidimiento
}	
