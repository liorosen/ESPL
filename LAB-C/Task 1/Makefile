all: mypipeline

mypipeline: mypipeline.o 
	gcc -g -m32 -o mypipeline mypipeline.o 

mypipeline.o: mypipeline.c 
	gcc -g -m32 -c mypipeline.c

clean:
	rm -f mypipeline mypipeline.o 