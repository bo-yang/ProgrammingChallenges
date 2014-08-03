#include <unistd.h>
#include "interpreter.h"

using namespace interp;

int main(int argc, char** argv) {
	 // Test code for loading insts from file
	string input;
	string output;
	int c;
	while((c=getopt(argc,argv,"i:o:"))!=-1) {
		switch(c) {
		case 'i':
			input=optarg;
			break;
		case 'o':
			output=optarg;
			break;
		default:
			abort();
		}
	}

	Interpreter intp;
	if(input.empty()) {
		cout<<"Please enter your instructions(type \"END\" to finish):"<<endl;
		intp.Load();
	} else {
		intp.Load(input);
	}
	intp.Run(); // run interpreters
	intp.Output();
	if(!output.empty())
		intp.Output(output);

	return 0;
}
