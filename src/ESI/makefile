CC = gcc

VPATH= ../shared

objects = esi.o ../shared/mySocket.o ../shared/buffer.o
pathLibraries = -lcommons -lparser

esi: $(objects)
	gcc  -o esi $(objects) $(pathLibraries) 

esi.o: mySocket.h 

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

clean:
	rm *.o

cleanShared:
	rm ../shared/*.o