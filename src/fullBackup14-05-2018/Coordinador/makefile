sources =  coordinador.c ../shared/mySocket.c ../shared/buffer.c ../shared/testConnection.c Coord-ESI/ESIHandling.c Coord-Instancia/InstanciaHandling.c Coord-Planificador/PlanificadorHandling.c 

pathLibraries = -lpthread -lcommons

coordinador: $(objects)
	gcc -g -o coordinador $(sources) $(pathLibraries)