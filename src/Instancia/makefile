
sources = instancia.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons

instancia: $(sources)
	gcc -g -o instancia $(sources) $(sharedLibraries) $(pathLibraries)
	cp instancia ../Instancia2

clean:
	rm instancia
