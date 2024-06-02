all: AntiVirus

AntiVirus: AntiVirus.o 
	gcc -g -m32 -o AntiVirus AntiVirus.o 

AntiVirus.o: AntiVirus.c 
	gcc -g -m32 -c AntiVirus.c

clean:
	rm -f AntiVirus AntiVirus.o 
