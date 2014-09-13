#include <iostream>
#include <ctime>
#include <unistd.h>
#include "question2.h"

// Class to test class TriangNum. 
class TriangNum_Test {
public:
	// constructor
	TriangNum_Test(int max_num=1000,bool neg=false):MAX_NUM(max_num),allow_neg_num(neg) {}
	~TriangNum_Test(){}

	// Generate random triangle numbers(both positive & negative)
	const vector<vector<int> >& GenTriangNums(int lines) {
		for(int i=0;i<lines;++i) {
			vector<int> layer;
			for(int j=0;j<i+1;++j) {
				int flag=1;
				if(allow_neg_num && rand()%2!=0) {
					flag=-1;
				}
				layer.push_back(flag* (rand() % MAX_NUM));
			}
			tnum.push_back(layer);
		}

		return tnum;
	}

	// Write generated triangle numbers into file
	void WriteToFile(string file) {
		fstream fs(file.c_str(),fstream::out);
		if(fs.is_open()) {
			for(int i=0;i<tnum.size();++i) {
				for(int j=0;j<tnum[i].size();++j) {
					fs<<tnum[i][j];
					if(j!=tnum[i].size()-1)
						fs<<"\t";
				}
				fs<<endl;
			}
		} else {
			cerr<<"Error: failed to create file "<<file<<endl;
			exit(1);
		}

		fs.close();
	}

private:
	vector<vector<int> > tnum;
	int MAX_NUM; // maximum numeber can be generated
	bool allow_neg_num; // is negative number is allowed or not
};

// 
// Main starts here
//
// To test class TriangNum, you can either
// 	(1) speficy a file of triangle numbers by option "-f <file>", or
//	(2) generate random triangle numbers by options "-l <#lines_triangle_tobe_generated> -m <maximum_numbers> -n <flag_allow_negative_numbers>". 
//  
int main(int argc, char** argv) {
	string file="numbers.txt";
	int lines=6;
	int max_num=100;
	bool generate=true;
	bool allow_neg_num=false;
	int c;
	while((c=getopt(argc,argv,"f:l:m:n"))!=-1) {
		switch(c) {
		case 'f':
			file=optarg;
			generate=false;
			break;
		case 'l':
			lines=atoi(optarg);
			break;
		case 'm':
			max_num=atoi(optarg);
			break;
		case 'n':
			allow_neg_num=true;
			break;
		default:
			cout<<"Usage: "<<argv[0]<<" [-f <file_triangle_numbers>] [-l <#lines_triangle_tobe_generated> -m <maximum_numbers> -n <flag_allow_negative_numbers>]"<<endl;
			abort();
		}
	}

	srand(time(NULL));

	if(generate) {
		TriangNum_Test test(max_num,allow_neg_num);
		test.GenTriangNums(lines);
		test.WriteToFile(file);
	}

	TriangNum tnum(file);
	tnum.PrintNum();
	cout<<"The lowest possible sum of numbers is: "<<tnum.ShortestPath()<<"(";
	tnum.PrintPath();
	cout<<")."<<endl;;

	return 0;
}
