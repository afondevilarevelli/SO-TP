sources = ../ESI/esi.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c ../ESI/parsi/src/parsi/parser.c
pathLibraries = -lcommons 

esi:
	gcc -g -o esi3 $(sources) $(sharedLibraries) $(pathLibraries)
	cp esi ../ESI2/esi

clean:
	rm esi
	rm *.o