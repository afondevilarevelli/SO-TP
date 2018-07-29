sources = instancia.c bibliotecaInstancia.c 
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons -lpthread

instancia: $(sources)
	gcc -g -o instancia $(sources) $(sharedLibraries) $(pathLibraries)
	cp instancia ../Inst2/instancia
	cp instancia ../Inst3/instancia	

clean:
	rm instancia
