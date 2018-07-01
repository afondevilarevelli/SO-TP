
Instancia en la cual se guardaría actualmente la clave (Calcular este valor mediante el algoritmo de
distribución(^4), pero sin afectar la distribución actual de las claves).
ESIs bloqueados a la espera de dicha clave.


//un esi necesita un conjunto de recursos
recursos#(claves)   ListaColas // si esta bloqueada

kill(int id){
list_element *lis =	ListaColas;
ESI_t *e ;

	while(lis != NULL){
		
		e =  buscarProcesoEnColas(lis-> cola,id);
		if(e != NULL){
			remuvo e de esta (lis->cola);
		}
		lis= lis -> sig;
	}					


// si se esta ejecutando
if(id == (procesoEjecutando()->id)){
liberar recursos(id /*del esi*/)
eliminar esi en ejecucion
}
}

status(char* clave){

ESI = procesoEjecutando()->id

if(tiene valor){
printf("mostrar valor %c",valor);
}
else printf("No hay valor");


if(ESI esta usando esa clave){
	printf("instancia de ese esi con id del esi");
}

/*p*/buscar clave en la ListaColas(char* clave) -> devuelva el elemento que consta de esta (clave ,cola)
if(si p != NULL){
	
	colaAsociada = (p-> cola) 
	ESI_t* t = NULL;
	//apuntar al primer elemento de la cola
while(t != NULL){
printf("Cola de esis %d",t->id);
}
}

}












