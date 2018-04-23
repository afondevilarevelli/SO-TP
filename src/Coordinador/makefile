CC = gcc
CFLAGS = -g
VPATH= ../shared

objects =  coordinador.o ../shared/mySocket.o ../shared/buffer.o ../shared/testConnection.o
pathLibraries = -lpthread -lcommons

coordinador: $(objects)
	gcc -g -o coordinador $(objects) $(pathLibraries)

coordinador.o: mySocket.h testConnection.h

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

clean:
	rm *.o

cleanShared:
	rm ../shared/*.o