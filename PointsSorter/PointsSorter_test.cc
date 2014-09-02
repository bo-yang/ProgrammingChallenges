#include <unistd.h>
#include <ctime>
#include "PointsSorter.h"

const int NUM_POINTS=1e6;
const int COORD_LIMIT=1e4;

class PointsSorter_Test:public PointsSorter {
public:
	PointsSorter_Test(std::string file) {
		GenPoints(file);
	}

	~PointsSorter_Test(){}

	// Generate random points for test 
	void GenPoints(std::string file) {
		for(int i=0;i<NUM_POINTS;++i) {
			Point3D pt;
			int pos_neg=pow(-1,rand()%2);
			pt.x=pos_neg*rand()%COORD_LIMIT;
			pos_neg=pow(-1,rand()%2);
			pt.y=pos_neg*rand()%COORD_LIMIT;
			pos_neg=pow(-1,rand()%2);
			pt.z=pos_neg*rand()%COORD_LIMIT;
			points.push_back(pt);
		}
		WritePointstoFile(file);
	}

	// Store generated points into file
	void WritePointstoFile(std::string file) {
		std::fstream fs(file,std::fstream::out);
		if(fs.is_open()) {
			for(auto& pt:points)
				fs<<pt.x<<"\t"<<pt.y<<"\t"<<pt.z<<std::endl;
		} else {
			std::cerr<<"Error: failed to open file "+file<<std::endl;
			abort();
		}

		fs.close();
	}
private:

};

int main(int argc, char** argv) {
	std::string usage="Usage: psorter -i <file_3d_points> -o <file_sorted_points> [-p] [-g]\n \
where\n \
\tfile_3d_points \t-- input file of 3D points to be sorted\n \
\tfile_sorted_points \t-- output file of the sorted points\n \
\t -p \t-- flag of printing sorted points to the monitor(false by default), optional\n \
\t -g \t-- flag of generating random points for test(false by default), optional\n \
";

	std::string input="sample_input.txt";
	std::string output="sample_output.txt";
	bool print_on_screen=false;
	bool gen_rand_points=false;

	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "hi:o:pg")) != -1)
		switch (c)
        {
        case 'p':
			print_on_screen = true;
			break;
		case 'g':
			gen_rand_points = true;
			break;
        case 'i':
			input = optarg;
			break;
        case 'o':
			output = optarg;
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

	srand(time(NULL));

	// Generate random points and write them into input
	if(gen_rand_points)
		PointsSorter_Test ptest(input);

	// Load and sort points
	PointsSorter psorter(input);
	psorter.SortPoints();
	psorter.WriteSortedPoints(output,print_on_screen);
}
