#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "AcquaintsFinder.h"

const int MAX_CALLS=1e6;
const int MAX_PHONE_NUM=15; 

class AcquaintsFinder_Test {
public:
	AcquaintsFinder_Test(std::string file) {
		GenCallLog(file);
	}

	void GenCallLog(std::string file){
		std::cout<<"Generating fake calls..."<<std::endl;
		std::fstream fs(file,std::fstream::out);
		if(fs.is_open()) {
			for(int i=0;i<MAX_CALLS;++i) {
				int calling=rand()%100000;
				int called=rand()%100000;
				fs<<calling<<", "<<called<<std::endl;
			}
		} else {
			std::cerr<<"Error: failed to open file "+file+"!"<<std::endl;
			abort();
		}
		fs.close();
	}
};

int main(int argc, char** argv) {
	std::string usage="Usage: finder -i <calls_log>\n";
	std::string input="acquaint_input.txt";
	bool gen_test_calls=false;

	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "hi:g")) != -1)
		switch (c)
        {
        case 'i':
			input = optarg;
			break;
		case 'g':
			gen_test_calls = true;
			break;
		case 'h':
			std::cout<<usage<<std::endl;
        	return 0;
        case '?':
			fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			std::cerr<<usage<<optopt<<std::endl;
        	return 1;
         default:
         	abort();
         }

	if(gen_test_calls) {
		srand(time(NULL));
		AcquaintsFinder_Test finder_test(input);
	}

	AcquaintsFinder finder(input);
	std::pair<std::string,int> most_acquaint=finder.FindMostAcquaint();
	std::cout<<most_acquaint.first<<" "<<most_acquaint.second<<std::endl;
}
