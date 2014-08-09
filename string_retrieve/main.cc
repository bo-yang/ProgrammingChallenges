#include <getopt.h>
#include <iostream>
#include <cstdlib>
#include "inferGenome.h"

int main(int argc, char** argv) {
	string input;
	string output;
	double p01=0.1;
	double p10=0.1;
	double prior1=0.5;

	int c;
	while (1) {
		static struct option long_options[] = {
			{"sham",    required_argument, 0, 's'},
			{"out",  	required_argument, 0, 'o'},
			{"p01",  	required_argument, 0, 'c'},
			{"p10",  	required_argument, 0, 'd'},
			{"prior1",  required_argument, 0, 'e'},
			{0, 		0, 				   0,  0 }
        };
       	/* getopt_long stores the option index here. */
       	int option_index = 0;
       	c = getopt_long (argc, argv, "a:b:c:d:e:",
                        long_options, &option_index);
 
       	/* Detect the end of the options. */
       	if (c == -1) {
         	break;
		}
       	
		switch (c) {
		case 's':
			input=optarg;
           	break;
        case 'o':
           	output=optarg;
           	break;
        case 'c':
           	p01=atof(optarg);
           	break;
        case 'd':
			p10=atof(optarg);
            break;
        case 'e':
			prior1=atof(optarg);
            break;
        case '?':
               /* getopt_long already printed an error message. */
               break;
        default:
               abort();
         }
    }

	InferGenome genome(p01,p10,prior1);
	genome.LoadReads(input);
	genome.CalcProbs();
	genome.WriteStat(output);
}
