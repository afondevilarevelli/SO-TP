sources = esi.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lcommons -lparsi

esi: $(sources) $(sharedLibraries)
	gcc -g -o esi $(sources) $(sharedLibraries) $(pathLibraries)
	cp esi ../ESI2/esi

clean:
	rm esi