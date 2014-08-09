#include "inferGenome.h"

// Constructor
InferGenome::InferGenome(double prob01=0.1, double prob10=0.1, double prob1=0.5) {
	p01=prob01;
	p10=prob10;
	p1=prob1;
	max_pos=0;
}

// Destructor
InferGenome::~InferGenome() {
	for(auto& ps:pos_stat) {
		delete ps.second;
	}
}

// Load all reads and do statistics
void InferGenome::LoadReads(string file) {
	fstream fs(file, fstream::in);
	if(fs.is_open()) {
		string line;
		while(std::getline(fs,line)) {
			// Read line-by-line
			size_t pos = 0;
			string delimiter="\t";
			if((pos = line.find(delimiter)) != std::string::npos) {
				int start_pos=stoi(line.substr(0,pos)); // start position of this raed
				string read=line.substr(pos+delimiter.length(),line.length()); // read
				ParseRead(start_pos,read);
				if(start_pos+read.length()-1 > max_pos) 
					max_pos=start_pos+read.length()-1; // record the detected max position
			} else {
				cerr<<"Warning: invalid format in file"<<file<<":"<<endl;
				cerr<<"\t"<<line<<endl;
			}
		}
	} else {
		cerr<<"Error: failed to open file "+file+"!"<<endl;
	}

	fs.close();
}

// count 1s and 0s in a read
void InferGenome::ParseRead(int start_pos, string& read) {
	for(int i=0;i<read.length();++i) {
		map<int,PosInfo*>::iterator got=pos_stat.find(start_pos+i);
		if(got==pos_stat.end()) {
			// no record for this position, create one
			PosInfo* pos=new PosInfo{start_pos+i,0,0,0.0};
			pos_stat[start_pos+i]=pos;
		}

		// update the record
		switch(read[i]) {
		case '0':
			pos_stat[start_pos+i]->nZero++;
			break;
		case '1':
			pos_stat[start_pos+i]->nOne++;
			break;
		default:
			cerr<<"Error: invalid nucleotide "<<read[i]<<endl;
			break;
		}
	} // end of for
}

// calculate probabilities
void InferGenome::CalcProbs() {
	// insert uncovered positions
	for(int i=0;i<=max_pos;++i) {
		map<int,PosInfo*>::iterator got=pos_stat.find(i);
		if(got==pos_stat.end()) {
			// no record found for this position, create one
			PosInfo* pos=new PosInfo{i,0,0,0.0};
			pos_stat[i]=pos;
		}
	}

	for(auto& ps:pos_stat) {
		ps.second->prob1=ProbOfOne(ps.second->nOne+ps.second->nZero, ps.second->nOne);
	}
}

// Write the probabilities to file
void InferGenome::WriteStat(string file) {
	fstream fs(file, fstream::out);
	if(fs.is_open()) {
		fs<<std::setprecision(3);
		fs<<std::fixed;
		// format: position \t probability_of_1
		for(auto& ps:pos_stat) 
			fs<<ps.first<<"\t"<<ps.second->prob1<<endl;
	}
	fs.close();
}

// Given n trials, calculate the probability of k 1s.
double InferGenome::ProbOfOne(int n, int k) {
	if (n == 0)
        return p1;

    double prob_zero = Binomial(n, p01, k); // probability a '0' would generate this k, given n
    double prob_one = Binomial(n, p10, n-k); // probability a '1' would generate, given n
    return prob_one/(prob_zero+prob_one);
}

// Bionomial distribution
double InferGenome::Binomial(int n, double p, int k) {
	return (double)Combination(n, k) * pow(p, k) * pow(1-p, n-k);
}

long InferGenome::Combination(int n, int k) {
	return (long)Factorial(n) / (Factorial(k) * Factorial(n - k));
}

long InferGenome::Factorial(int n) {
	long result = 1;
    for (int i = 2; i <= n; i++)
        result *= i;
    return result;
}
