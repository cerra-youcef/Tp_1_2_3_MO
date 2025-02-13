#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "TP1Functions.h"


int main(int argc, char **argv)
{
	int rval =0;
	//File instance name
	//-F option
        char instance_file[1024];
        snprintf(       instance_file,
                        1024,
                        "%s",
                        "instance3.csv");

	char c;
        while ((c=getopt (argc, argv,"F:h")) != EOF)
	{
		switch(c)
		{
                        case 'F':
				snprintf(       instance_file,
						1024,
						"%s",
						optarg);
				break;

			case 'h':
				fprintf(stderr,"Usage: ./TP1 [options]\nOptions:\n\n");
				fprintf(stderr,"******** INSTANCE DATA ********\n");
				fprintf(stderr,"\t-F Instance file name to load............................(default %s).\n",instance_file);
				break;
			default:
				exit(0);
		}
	}

	dataSet data;

	//Open the instance file
	FILE* fin = fopen(instance_file,"r");
	read_TP1_instance(fin,&data);
	fclose(fin);


	//execute your solution methods on the instance you just read
	//TP1
	//KP_greedy(&data);
	//KP_LP(&data);


	//TP2
	//dynamic programmation
    //Recursive_Dynamic_programming(&data);
    //TP3
    // Perform variable preprocessing
    knapsack_variable_preprocessing(&data);




	return rval;
}

