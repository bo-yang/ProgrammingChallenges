#ifndef _QUESTION2_H_
#define _QUESTION2_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

// Data structure of Triangle numbers.
class TriangNum {
public:
	TriangNum();	// default constructor
	TriangNum(string file);	// constructor
	~TriangNum(){}

	const vector<vector<int> >& LoadData(string file); // Build a binary tree from Triangle Numbers
	int ShortestPath();	// Find the path of lowest possible sum of numbers
	void PrintNum() const;
	void PrintPath() const;

private:
	vector<vector<int> > tnum;	// triangle numbers
	vector<vector<int> > trace; // all possible sums in the shortest path
	vector<int> spath; // shortest path

	vector<int> ReadNumbers(string line); // Read numbers in a line
	int Min(int x, int y) { return x<y?x:y; }
};


// default constructor
TriangNum::TriangNum() {
}

// constructor
TriangNum::TriangNum(string file) {
	LoadData(file);
}

// Load triangular numbers from file
const vector<vector<int> >& TriangNum::LoadData(string file) {
	fstream fs(file.c_str(),fstream::in);
	if(fs.is_open()) {
		// read config line-by-line
		string line;
		while(std::getline(fs,line)) {
			// read numbers line by line
			vector<int> layer=ReadNumbers(line);
			if(!layer.empty())
				tnum.push_back(layer);

		} // end while
	} else {
		cerr<<"Error: failed to open file "<<file<<endl;
		exit(1);
	}

	fs.close();

	return tnum;
}

// Read numbers(separated by tabs) in a line
vector<int> TriangNum::ReadNumbers(string line) {
	vector<int> nums;
	string delimiter="\t";
	size_t pos=0;
	while((pos = line.find(delimiter)) != std::string::npos) {
		string n=line.substr(0,pos);
		nums.push_back(atoi(n.c_str()));
		line.erase(0,pos+delimiter.length());
	}
	if(!line.empty())
		nums.push_back(atoi(line.c_str()));

	return nums;

}

// Find the path of lowest possible sum of numbers and the corresponding shortest path.
// The lowest sum can be found recursively by choosing the child node with the 
// smallest sum. This greedy strategy should work for both positive and negative 
// numbers, because every possible combination of nearby numbers are considered 
// in the recursion. 
//
// The recursion function is:
// 		OPT(i,j)=tnum(i,j)+min(OPT(i+1,j),OPT(i+1,j+1))
//
// where OPT(i,j) is the lowest sum of number tnum[i][j]. 
// Above function can be implemented by dynamic programming.
//
// Given a triangle number with n lines, then
// 	Time complexity for finding the lowest sum of numbers: O(n^2).
// 	Time complexity for finding the shortest path: O(n).
int TriangNum::ShortestPath() {
	// Find the lowest possible sum of numbers.
	trace=tnum;
	for(int i=tnum.size()-2;i>=0;--i) {
		for(int j=tnum[i].size()-1;j>=0;--j) {
			trace[i][j]=tnum[i][j]+Min(trace[i+1][j],trace[i+1][j+1]);
		}
	}

	// Find the shortest path
	spath.push_back(tnum[0][0]);
	int idx=0;
	for(int i=1;i<tnum.size();++i) {
		if(trace[i][idx]<trace[i][idx+1]) {
			spath.push_back(tnum[i][idx]);
		} else {
			idx+=1;
			spath.push_back(tnum[i][idx]);
		}
	}

	return trace[0][0];
}

// Print triangular numbers
void TriangNum::PrintNum() const {
	cout<<"Triangular numbers:"<<endl;
	for(int i=0;i<tnum.size();++i) {
		for(int j=0;j<tnum[i].size();++j) {
			cout<<tnum[i][j]<<" ";
		}
		cout<<endl;
	}
}

// Print the shortest path
void TriangNum::PrintPath() const {
	for(int i=0;i<spath.size();++i) {
		if(spath[i]>=0)
			cout<<spath[i];
		else
			cout<<"("<<spath[i]<<")";
		if(i!=spath.size()-1)
			cout<<"+";
	}
}

#endif // _QUESTION2_H_
