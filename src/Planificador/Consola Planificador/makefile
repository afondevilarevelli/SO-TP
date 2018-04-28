CC = gcc
VPATH= ../shared

objects =  planificador.o ../shared/mySocket.o ../shared/protocolo.o 
pathLibraries = -lcommons -lreadline

planificador: $(objects)
	gcc -o planificador $(objects) $(pathLibraries)

planificador.o: mySocket.h protocolo.h

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

../shared/protocolo.o: protocolo.h

clean:
	rm *.o planificador

cleanShared:
	rm ../shared/*.o

