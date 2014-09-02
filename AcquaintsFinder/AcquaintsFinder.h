#ifndef _ACQUAINTSFINDER_H_
#define _ACQUAINTSFINDER_H_

// This class parses phone calls and finds out the number with most acquaintances.
//
// Phone calls are stored in a hash table(HashCall calls). For each entry, the key is
// the calling number, and the value is a hash table of called numbers(friends of the
// calling number). The reason for storing friends in another hash table is that finding
// an element in unordered_map costs constant time on average. 
//
// The acquaitances for each calling number is counted iteratively: given a calling 
// number, go through all of its friends' friends. For calling number A, one of A's 
// frind B and one of B's frind C, if C not in A's friend list and A is not in C's 
// friend list, either, then A and C are acquaintances. All phone numbers and the 
// corresponding numbers their acquaintances are stored in a hash table.
//
// Complexity - givn n calls:
// The time complexity of reading call log is O(n), finding friendships & acquaintances
// costs O(1) on average(but O(n^3) at the worst case, due to C++ unordered_map) and
// finding the number with most acquaintances requires O(n). Therefore, on average the
// time complexity is O(n).
// The space complexity is also O(n), because each number is stored for 3 times at most.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <cctype>
#include <locale>

typedef std::unordered_map<std::string,std::unordered_map<std::string,bool> > HashCall;
typedef std::unordered_map<std::string,int> HashAcq;

class AcquaintsFinder {
public:
	AcquaintsFinder(){}
	AcquaintsFinder(std::string log){LoadCallLog(log);}
	~AcquaintsFinder(){}

	HashCall& LoadCallLog(std::string log);
	bool IsFriend(std::string a, std::string b);	// determine if b is a friend of a
	std::pair<std::string,int> FindMostAcquaint();	// find the one with most acquaints

protected:
	std::string& ltrim(std::string &s); // trim left spaces of a string
	std::string& rtrim(std::string &s); // trim right spaces of a string
	std::string& trim(std::string &s);  // trim spaces in a string

private:
	HashCall calls; // <calling #, vector<called #> >
	HashAcq acquaints;	// number of acquaints for each caller
};

// trim from start
inline std::string& AcquaintsFinder::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string& AcquaintsFinder::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string& AcquaintsFinder::trim(std::string &s) {
    return ltrim(rtrim(s));
}


// Constructor
HashCall& AcquaintsFinder::LoadCallLog(std::string log) {
	std::fstream fs(log,std::fstream::in);
	if(fs.is_open()) {
		std::string line;	
		while(std::getline(fs,line)) {
			// a line is similar to "123456, 222"
			std::size_t pos=line.find(",");
			if(pos==std::string::npos) {
				std::cerr<<"Error: invalid line "<<line<<" found in "<<log<<std::endl;
				continue;
			}

			std::string calling=line.substr(0,pos); // substring before ','
			trim(calling); // trim leading & tailing spaces
			std::string called=line.substr(pos+1);	// substring after ','
			trim(called);  // trim leading & tailing spaces
			HashCall::iterator got=calls.find(calling);
			if(got==calls.end()) {
				// No record as a key
				std::unordered_map<std::string,bool> called_hash;
				called_hash[called]=true;
				calls[calling]=called_hash;
			} else {
				// Has record as key
				calls[calling][called]=true;
			}
		}
	} else {
		std::cerr<<"Error: failed to open file "+log+"!"<<std::endl;
		abort();
	}
}

// Determine if a and b are friends
bool AcquaintsFinder::IsFriend(std::string a, std::string b) {
	HashCall::iterator got1=calls.find(a);
	if(got1!=calls.end()){
		// Check if b is in a's friend list
		std::unordered_map<std::string,bool>::iterator got2=calls[a].find(b);
		if(got2!=calls[a].end())
			return true; // b is a friend of a
	}

	got1=calls.find(b);
	if(got1!=calls.end()){
		// Check if a is in b's friend list
		std::unordered_map<std::string,bool>::iterator got2=calls[b].find(a);
		if(got2!=calls[b].end())
			return true; // a is a friend of b
	}

	return false;
}

// Find the one with most acquaints. Since the hash table is used to store the
// calls and friends, the time complexity of counting should be O(1) on average.
std::pair<std::string,int> AcquaintsFinder::FindMostAcquaint() {
	// Count the number of acquaintants for each calling #
	for(auto& c:calls) { // go through all calls
		for(auto& f:c.second) {	// check every friend of current calling #
			HashCall::iterator got1=calls.find(f.first);
			if(got1!=calls.end()) {
				for(auto& aqt:calls[f.first]) { // check every friend's friend
					if(!IsFriend(c.first,aqt.first)) {
						HashAcq::iterator got2=acquaints.find(c.first);
						if(got2==acquaints.end())
							acquaints[c.first]=1;
						else
							acquaints[c.first]++;
						got2=acquaints.find(aqt.first);
						if(got2==acquaints.end())
							acquaints[aqt.first]=1;
						else
							acquaints[aqt.first]++;
					}
				} // end for
			}
		} // end for
	} // end for

	// Find the numbers with the most acquaintance using a heap
	struct greaterPair{
		bool operator() (std::pair<std::string, int>& x, std::pair<std::string, int>& y) {return x.second > y.second;}
	};
	//// heap is used here just for the convenience of expanding to finding N items ////
	std::priority_queue<std::pair<std::string, int>,std::vector<std::pair<std::string, int> >, greaterPair> heap;
	const int heap_len=1; 	
	for(auto& ac:acquaints) {
		if(heap.size()<heap_len){
			heap.push(std::pair<std::string,int>(ac.first,ac.second));
		} else if(heap.top().second<ac.second) {
			heap.pop(); // keep the heap small 
			heap.push(std::pair<std::string,int>(ac.first,ac.second));
		}
	} // end for
	while(heap.size()>1) {
//		std::cout<<heap.top().first<<", "<<heap.top().second<<std::endl; // TEST ONLY
		heap.pop();
	}

	return heap.top();
}

#endif
