// #include "ESIHandling.h"
// #include "bibliotecaInstancia.h"

void kill(int id){
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

void status(char* clave){
	valor(clave);
	buscarInstancia(clave);//posicion de memoria en donde se encuentra guardado esa clave
	listar(clave);//muestra los esis que esperan esa clave e imprime "no hay esis esperando" si no hay
}

void valor(char* clave){
	t_entrada_tabla * pEntry = findEnTablaEntrada(clave);
	if(pEntry != NULL){
		char*valor = obtenerValor(clave);
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
	printf("FAlta");
}

bool esiPerteneceCola(t_queue* cola,int id){
	ESI_t* e = buscarProcesoEnColas(cola,id);
	return (e != NULL);
}


void deadlock(){
	ESI_t* e1;	int id1
	ESI_t* e2;

	t_link_element* p = ListaColas -> head ;
	t_link_element* q
	cola_clave* c1;
	cola_clave* c2;
	
	while (p != NULL){
		q = p -> next;
		c1 = (cola_clave*)(p -> data);
		id1 = c1 -> idEsiUsandoClave ;
		while(q != NULL){
			c2 = (cola_clave*)(q -> data);
			id2 = c2 -> idEsiUsandoClave ;
				if(esiPerteneceCola(c1,id2) && esiPerteneceCola(c2,id1) &&	id1 != id2){
					printf("Deadblock de esis %d y %d",id1,id2);
				}
			q = q -> next ;
		}
	 p = p -> next	
	}
	printf("No hay clave(recurso) que un ESI use y que otro ESI espere este recurso");
}
