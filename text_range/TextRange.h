#include <iostream>
#include <string>
#include <vector>
#include <cstddef>	// std::size_t
#include <cstdlib>
#include <algorithm>

using namespace std;

// Data structure for a single range
struct Range {
public:
	char linc;	// left-side(lower-bound) inclusion, '(' or '['
	char rinc;	// right-side(upper-bound) inclusion, ')' or ']'
	string lower;	// lower boundaries
	string upper;	// upper boundaries

	Range(){}	// default constructor
	Range(string str);	// constructor
private:
	void FormatError(string str);	// Print format error
};

// Data structure for a text range
class TextRange {
public:
	TextRange();
	~TextRange();
	TextRange(string str); // Build ranges from string

	TextRange operator+(const TextRange& rng); // Addition of a text Range to the set being tracked.
	TextRange operator+(const Range& rng); // Addition of a text Range to the set being tracked.
	TextRange& operator+=(const TextRange& rng);
	TextRange& operator+=(const Range& rng);
	TextRange operator-(const TextRange& rng); // Deletion of a text Range from the set being tracked.
	TextRange operator-(const Range& rng);
	TextRange& operator-=(const TextRange& rng);
	TextRange& operator-=(const Range& rng);
	bool InRange(const TextRange& rng) const; // Query on whether a range is inside the set of Ranges.
	bool InRange(const Range& rng) const;
	bool InRange(const string str) const; // Query on whether a string is inside the set of Ranges being tracked.
	
	int GetRangeNum() const {return range.size();}
	const vector<Range>& GetRange() const {return range;}	// Get a range by index
	void Print() const;	// Print ranges
	string ToString() const;	// convert to string

protected:
	void Sort();	// Sort ranges by the lower boundaries

private:
	vector<Range> range;	// ranges

	string Min(string a, string b){return a>b ? b : a;}
	string Max(string a, string b){return a>b ? a : b;}
	static bool cmpFunction(Range& x, Range& y) {return x.lower < y.lower;}
};


// Build a single range from string, like "[AaA - bb)"
Range::Range(string str) {
	// Parse string
	string delimiter="-";
	size_t pos=0;
	if((pos = str.find(delimiter)) != std::string::npos) {
		string left=str.substr(0,pos);
		str.erase(0,pos+delimiter.length());
		string right=str;

		// left part
		string whitespaces(" \t\f\v\n\r");
		pos=left.find_first_of("([");
		if(pos!=std::string::npos) {
			linc=left[pos];
			left.erase(0,pos+1);	// trim leading spaces
			pos=left.find_last_not_of(whitespaces);
			if(pos!=std::string::npos)
				left.erase(pos+1);	// trim ending spaces
			lower=left;
		} else {
			FormatError(str);
		}

		// right part
		pos=right.find_first_not_of(whitespaces);
		if(pos!=std::string::npos) {
			right.erase(0,pos);	// trim leading spaces
			pos=right.find_last_of(")]");
			if(pos!=std::string::npos)
				rinc=right[pos];	
			else {
				FormatError(right);
			}
			right.erase(pos);	// trim ending spaces
			upper=right;
		} else {
			FormatError(right);
		}
	} else {
		// For string like "AbC", set a default upper boundary.
		linc='[';
		rinc=']';
		lower=str;
		upper=str;
	}
}

inline void Range::FormatError(string str) {
	string err_msg="ERROR: invalid text range type: ";
	cerr<<err_msg<<str<<endl;
	exit(1);
}

// Default constructor
TextRange::TextRange() {
	// empty range
}

// Default destructor
TextRange::~TextRange() {

}

//
// The input string may contain several ranges, and each range is separated by "-". 
// An example of input string is:
// 		"[AaA - bb),[Dd-df]"
//
TextRange::TextRange(string str) {
	// Parse string and initialize TextRange
	string delimiter=",";
	size_t pos=0;
	while((pos = str.find(delimiter)) != std::string::npos) {
		string r=str.substr(0,pos);
		str.erase(0,pos+delimiter.length());
		Range rng(r);
		*this += rng;
	}
	if(!str.empty()) {
		Range rng(str);
		*this += rng;
	}
	//this->Sort();
}

// Addition:
// 	1. If the two single ranges have intersections, then merge the lower and upper boundaries.
// 	E.g.,
// 		[AaA-BaB] + [Aac-CaC] = [AaA-CaC].
// 		[AaA-bb] + [CAC-df] = [AAA-df].
// 	2. If the two single ranges are independent, then keep both of them. E.g.,
// 		[AaA-CaC] + [Dd-Df] = [AaA-CaC],[Dd-Df].
// 	3. For multiple ranges adding single/multiple ranges, compare all the lower and upper
// 	boundaries and merge/add them.
//
// To determine if two single ranges have intersections or not, we need to compare the lower 
// boundary with the upper boundary and vice versa.
//
// ASSUME that the lower and upper boundaries can be campared using string comparison rules.
TextRange TextRange::operator+(const TextRange& rng) {
	TextRange tmp=*this;
	const vector<Range>& newRng=rng.GetRange();
	for(int i=0;i<rng.GetRangeNum();++i) {
		tmp += newRng[i];
	}

	return tmp;
}

// Add a single range
TextRange TextRange::operator+(const Range& rng) {
	TextRange tmp=*this;
	Range newRng=rng;
	vector<Range>::iterator it=tmp.range.begin();
	while(it!=tmp.range.end()) {
		if(newRng.upper < it->lower) {
			tmp.range.insert(it, newRng);
			return *this;
		} else if(newRng.lower > it->upper) {
			it++;
			continue;
		} else {
			newRng.lower=Min(newRng.lower,it->lower);
			newRng.upper=Max(newRng.upper,it->upper);
			it=tmp.range.erase(it);
		}
	}
	tmp.range.push_back(newRng);

	return tmp;
}

// Compound assignment
TextRange& TextRange::operator+=(const TextRange& rng) {
	*this = *this + rng;
	return *this;
}

// Compound assignment
TextRange& TextRange::operator+=(const Range& rng) {
	*this = *this + rng;
	return *this;
}

// Substraction:
// 	1. If the two single ranges have intersections, then substract the common parts.
// 	E.g.,
// 		[AaA-CaC] - [bb - CA] = [AaA - bb), (CA - CaC]
// 	2. If the two single ranges are independent, then keep the range unchanged. E.g.,
// 		[AaA-CaC] - [Dd-Df] = [AaA-CaC].
// 	3. For multiple ranges substracting single/multiple ranges, compare all the lower and upper
// 	boundaries and merge/add them.
//
// ASSUME that the lower and upper boundaries can be campared using string comparison rules.
TextRange TextRange::operator-(const TextRange& rng) {
	TextRange tmp=*this;
	const vector<Range>& newRng=rng.GetRange();
	for(int i=0;i<rng.GetRangeNum();++i) {
		tmp -= newRng[i];
	}

	return tmp;
}

// Substraction
TextRange TextRange::operator-(const Range& newRng) {
	TextRange tmp=*this;
	vector<Range>::iterator it=tmp.range.begin();
	while(it!=tmp.range.end()) {
		if(newRng.upper>it->lower && newRng.upper<it->upper) {
			it->lower=newRng.upper;
		} else if(newRng.lower>it->lower && newRng.lower<it->upper) {
			it->upper=newRng.lower;
		} else if(newRng.lower<it->lower && newRng.upper>it->upper) {
			it=tmp.range.erase(it);
		} else if(newRng.lower>it->lower && newRng.upper<it->upper){
			// Separate this range into two parts
			// Part 1
			Range tmpRng;
			tmpRng.linc=it->linc;
			tmpRng.lower=it->lower;
			tmpRng.upper=newRng.lower;
			if(newRng.linc=='(')
				tmpRng.rinc=']';
			else
				tmpRng.rinc=')';
			tmp.range.insert(it,tmpRng);
			tmp.Print(); // TEST ONLY

			// Part 2
			it++;
			if(newRng.rinc==')')
				tmpRng.linc='[';
			else
				tmpRng.linc='(';
			tmpRng.lower=newRng.upper;
			tmpRng.upper=it->upper;
			tmpRng.rinc=it->rinc;
			tmp.range.insert(it,tmpRng);
			tmp.Print(); // TEST ONLY

			// Delete the original range
			it++;
			it=tmp.range.erase(it);
		} else {
			it++;
		}
	}

	return tmp;
}

// Compound assignment
TextRange& TextRange::operator-=(const TextRange& rng) {
	*this = *this - rng;
	return *this;
}

// Compound assignment
TextRange& TextRange::operator-=(const Range& rng) {
	*this = *this - rng;
	return *this;
}

// Query:
// 	1. Given a single range, if both the lower boundary and the upper boundary are
// 	in this range, and the whole given range is covered in this range, then return true. 
// 	Otherwise, return false.
//	2. Given multiple ranges, only if every single range is in this range, return true.
//	Otherwise, return false.
bool TextRange::InRange(const TextRange& rng) const {
	bool ret=true;
	const vector<Range>& newRng=rng.GetRange();
	for(int i=0;i<rng.GetRangeNum();++i) {
		if(!InRange(newRng[i])) {
			ret=false;
			break;
		}
	}

	return ret;
}

// Query:
// 	Given a single range, if both the lower boundary and the upper boundary are located
// 	in this range, and the whole given range is covered in this range, then return true. 
// 	Otherwise, return false.
bool TextRange::InRange(const Range& rng) const {
	bool ret=false;
	for(int j=0;j<GetRangeNum();++j) {
		if(rng.lower>=range[j].lower && rng.upper<=range[j].upper) {
			if(!( (rng.lower==range[j].lower && range[j].linc!=rng.linc) || 
						(rng.upper==range[j].upper && range[j].rinc!=rng.rinc) )) {
				ret=true;
				break;
			}
		}
	} // end for

	return ret;
}

// Query
bool TextRange::InRange(const string str) const {
	Range rng(str);
	return InRange(rng);
}

// Print text range
inline void TextRange::Print() const {
	for(int i=0;i<GetRangeNum();++i) {
		cout<<range[i].linc<<range[i].lower<<"-"<<range[i].upper<<range[i].rinc;
		if(i<GetRangeNum()-1)
			cout<<",";
	}
	cout<<endl;
}

// Convert range to string
inline string TextRange::ToString() const {
	string str="";
	for(int i=0;i<GetRangeNum();++i) {
		str=str+range[i].linc+range[i].lower+"-"+range[i].upper+range[i].rinc;
		if(i<GetRangeNum()-1)
			str+=",";
	}

	return str;
}

// Sort ranges according to the lower boundaries
void TextRange::Sort() {
	std::sort(range.begin(),range.end(),cmpFunction);
}
