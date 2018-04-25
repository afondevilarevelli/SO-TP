CC = gcc
CFLAGS = -g
VPATH= ../shared

objects = instancia.o ../shared/mySocket.o ../shared/buffer.o

instancia: $(objects)
	gcc -g -o instancia $(objects)

instancia.o: mySocket.h protocolo.h

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

clean:
	rm *.o

cleanShared:
	rm ../shared/*.o