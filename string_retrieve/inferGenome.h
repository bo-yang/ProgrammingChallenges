#ifndef _INFERGENOME_H_
#define _INFERGENOME_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <iomanip>

using namespace std;

const int MAX_READ_LEN=1000;

struct PosInfo {
	int pos;	// position
	int nOne;	// number of ones
	int nZero;	// number of zeros
	double prob1;	// probability of being a '1' at this position
};

class InferGenome {
public:
	InferGenome(double prob01, double prob10, double prob1);
	~InferGenome();

	void LoadReads(string file);	// Load all reads and do statistics
	void WriteStat(string file);	// Write the probabilities to file
	void ParseRead(int start_pos, string& read);	// count 1s and 0s in a read
	void CalcProbs();	// calculate probabilities

protected:
	double ProbOfOne(int n, int k); // Given occurrences n(both 1s and 0s), calculate the probability there are k 1s.
	double Binomial(int n, double p, int k);
	long Combination(int n, int k);
	long Factorial(int n);

private:
	double p01;	// Probability a '0' base is measured as a 1
	double p10;	// Probability a '1' base is measured as a 0
	double p1;	// prior probability that any given base is a 1
	map<int,PosInfo*> pos_stat;	// position-based statistics
	unsigned int max_pos;	// the max position found in reads
};

#endif // _INFERGENOME_H_
