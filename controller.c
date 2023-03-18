#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

/* Insert extern modules here */

extern void controllerasm(char * bufferin, char * bufferout);

/* ************************************* */

enum { MAXLINES = 400 };

long long current_timestamp() {
    struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	/* te.tv_nsec nanoseconds divide by 1000 to get microseconds*/
	long long nanoseconds = tp.tv_sec*1000LL + tp.tv_nsec; // caculate nanoseconds  
    return nanoseconds;
}


int main(int argc, char *argv[]) {
    int i = 0;
    char bufferin[MAXLINES*8+1] ;
    char line[1024] ;
    long long tic_c, toc_c, tic_asm, toc_asm;

    char bufferout_c[MAXLINES*8+1] = "" ;
    char bufferout_asm[MAXLINES*8+1] = "" ;

    FILE *inputFile = fopen(argv[1], "r");

    if(argc != 3)
    {
        fprintf(stderr, "Syntax ./test <input_file> <output_file>\n");
        exit(1);
    }

    if (inputFile == 0)
    {
        fprintf(stderr, "failed to open the input file. Syntax ./test <input_file> <output_file>\n");
        exit(1);
    }

    while (i < MAXLINES && fgets(line, sizeof(line), inputFile))
    {
        i = i + 1;
        strcat( bufferin, line) ;
    }
	
    bufferin[MAXLINES*8] = '\0' ;

    fclose(inputFile);


    /* C Elaboration */
    tic_c = current_timestamp();

  	int c = 0, j = 0;
  	int init, reset, ph, nck=0 ;
  	char st = '-', vlv[3] = "--", oldst = '-' ;
  	char tmpout[9] ;
  	char nckstr[3] = "--" ;

  	i = 0;


  	while ( bufferin[i] != '\0' ) {
		init = bufferin[i] - '0' ;
		reset = bufferin[i+2] - '0' ;
    	ph = (bufferin[i+4]-'0')*100 + (bufferin[i+5]-'0')*10 + (bufferin[i+6]-'0') ;

		strcpy(tmpout, "-,--,--\n") ;
      
		/* printf("i=%d, init: %d, reset: %d, ph: %d, tmpout: %s", i, init, reset, ph, tmpout) ; */
    	if ( init == 0 || reset == 1) {
          oldst = '-' ;
    	}
    	else
    	{
      		/* Calculate the actual pH */
      		if ( ph < 60 ) {
      			st = 'A' ;
      		}
      		else if ( ph >= 60 && ph <= 80 ) {
        		st = 'N' ;
      		}
      		else if ( ph > 80 ) {
        		st = 'B' ;
            }

      		/* update the clock counter */
      		if ( st != oldst ) {
        		nck = 0 ;
      		}
      		else
      		{
        		nck = nck + 1 ;
      		}
      		sprintf(nckstr,"%.2d",nck) ; 
			/* decide the state of the valve */
      		if ( st == 'A' && nck>4 ) {
        		strcpy(vlv,"BS") ;
			}
      		else if ( st == 'B' && nck>4 ) {
            strcpy(vlv,"AS") ;
            }
      		else {
            strcpy(vlv,"--") ;
            }
      		/* uupdate oldst */
      		oldst = st ;
      		/* generta the output string */
      		tmpout[0] = st ;
      		tmpout[2] = nckstr[0] ;
      		tmpout[3] = nckstr[1] ;
      		tmpout[5] = vlv[0] ;
      		tmpout[6] = vlv[1] ;
    	}

    	strcat( bufferout_c, tmpout) ;
    	i = i + 8 ;
  	}

    toc_c = current_timestamp();

  	long long c_time_in_nanos = toc_c - tic_c;

    /* END C ELABORATION */


    /* START ASM ELABORATION */

    tic_asm = current_timestamp();

    /* Assembly inline:
	Place your inline assembly code block or assembly function call here.
    The code block takes 'bufferin' as input and must return a string variable 'bufferout_asm' which will then be saved to file. */
	
	controllerasm(bufferin, bufferout_asm);

    toc_asm = current_timestamp();

  	long long asm_time_in_nanos = toc_asm - tic_asm;

    /* END ASM ELABORATION */


    printf("C time elapsed: %lld ns\n", c_time_in_nanos);
    printf("ASM time elapsed: %lld ns\n", asm_time_in_nanos);

    /* Save ASM result */
  	FILE *outputFile;
    outputFile = fopen (argv[2], "w");
    fprintf (outputFile, "%s", bufferout_asm);
    fclose (outputFile);

    return 0;
}
