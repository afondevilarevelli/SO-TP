sources = esi.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c ./parsi/src/parsi/parser.c
pathLibraries = -lcommons 

esi:
	gcc -g -o esi $(sources) $(sharedLibraries) $(pathLibraries)
	cp esi ../ESI2/esi

clean:
	rm esi
	rm *.o
