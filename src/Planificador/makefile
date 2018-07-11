sources = CoordHandling/CoordHandling.c planificador.c ESIHandling/ESIHandling.c consolaPlanificador.c  algoritmosDePlanificacion.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons -lpthread -lreadline

planificador: $(sources)
	gcc -g -o planificador $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm planificador
