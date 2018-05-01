CC = gcc
VPATH = ../shared

objects =  planificador.o ../shared/mySocket.o ../shared/buffer.o 
pathLibraries = -lcommons

planificador: $(objects)
	gcc -o planificador $(objects) $(pathLibraries) 

planificador.o: mySocket.h 

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

clean:
	rm *.o

cleanShared:
	rm ../shared/*.o