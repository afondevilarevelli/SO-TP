CC = gcc
CFLAGS = -g
VPATH= ../shared

objects = ../shared/mySocket.o ../shared/buffer.o ../shared/testConnection.o

testClient: testClient.o $(objects)
	gcc -g -o testClient testClient.o $(objects)

testServer: testServer.o $(objects)
	gcc -g -o testServer testServer.o $(objects)

coordinador.o: mySocket.h testConnection.h

../shared/testConnection.o: testConnection.h mySocket.h

../shared/mySocket.o: mySocket.h buffer.h

../shared/buffer.o: buffer.h

clean:
	rm *.o

cleanShared:
	rm ../shared/*.o
	
all: testClient testServer