
sources = instancia.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons

instancia: $(sources)
	gcc -g -o instancia $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm instancia
