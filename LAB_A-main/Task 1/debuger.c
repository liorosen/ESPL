#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int _sum(int a, int b);
void getNumbers(int* a, int* b);

int main(int argc, char **argv) {
  int i,a,b,c;
  FILE * output = stdout;
  int verbose = 0;
  int debugMode = 1 ; // Default debug mode is in "ON" condition
  
  for(i = 1 ; i < argc ; i++ ){
    if(strcmp(argv[i] , "-o" ) == 0){
	          output = fopen(argv[++i] , "w");
    } else if(strcmp(argv[i] , "-v" ) == 0 ){
	          verbose = 1;
    } else if (strcmp(argv[i], "-D" ) == 0) {
            debugMode = 0;
    } else if (strcmp(argv[i] , "+D" ) == 0) {
            debugMode = 1;
    } else {
        	  printf( "invalid parameter - %s\n" , argv[i] );
	          return 1;
    }

    // Print command-line arguments if in debug mode
    if (debugMode) {
        fprintf(stderr , "Argument[%d]: %s\n", i , argv[i]);
    }
  }
  getNumbers(&a,&b);
  c = _sum(a,b);
  if(verbose){
	    fprintf(output, "Welcome to Part 0, the input numbers are %i and %i\nThe sum is %i\n", a , b , c);
  }
  fprintf(output,"sum of %i and %i is: %i\n", a , b , c );
  if(output!=stdout){
      fclose(output);
  }
  return 0;
}
