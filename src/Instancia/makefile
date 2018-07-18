sources = instancia.c bibliotecaInstancia.c 
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons -lpthread

instancia: $(sources)
	gcc -g -o instancia $(sources) $(sharedLibraries) $(pathLibraries)
	

clean:
	rm instancia
