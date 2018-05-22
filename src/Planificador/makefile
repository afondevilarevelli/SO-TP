sources = planificador.c ESIHandling/ESIHandling.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons -lpthread

planificador: $(sources)
	gcc -g -o planificador $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm planificador
