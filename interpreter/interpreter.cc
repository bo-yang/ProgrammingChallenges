#include "interpreter.h"

using namespace interp;

// constructor
SingleInterp::SingleInterp():ic(0),ip(0),cn(-1),stat(init) {
	for(int i=0;i<REGSIZE;++i)
		reg.push_back(0);

	for(int i=0;i<RAMSIZE;++i)
		ram.push_back("000");
}

// constructor
SingleInterp::SingleInterp(int case_no,vector<string>& insts):ic(0),ip(0),cn(case_no),stat(init) {
	for(int i=0;i<REGSIZE;++i)
		reg.push_back(0);

	for(int i=0;i<RAMSIZE;++i)
		ram.push_back("000");

	assert(Load(insts)==true);
}

// destructor
SingleInterp::~SingleInterp() {
	// TBD
}

inline int SingleInterp::GetIC() {
	return ic;
}

// return the case number
inline int SingleInterp::GetCN() {
	return cn;
}

// get the status of the interpreter
inline IntpState SingleInterp::State() {
	return stat;
}

// load instructions
bool SingleInterp::Load(vector<string>& insts) {
	if(insts.size()>RAMSIZE)
		return false;

	for(int i=0;i<insts.size();++i)
		ram[i]=insts[i];

	return true;
}

// execute an instrution
bool SingleInterp::ExecInst(string inst) {
	cout<<"Executing "<<inst<<" ..."<<endl; // TEST ONLY

	char l=inst[0];	// left-most character of inst
	char m=inst[1]; // middle character of inst
	char r=inst[2]; // right character of inst
	int il=inst[0]-'0';
	int im=inst[1]-'0';
	int ir=inst[2]-'0';

	switch(l) {
		case '0':	// conditional goto
			if(reg[ir]!=0)
				ip=reg[im];
			else
				ip++;
			break;
		case '1':	// halt
			stat=halt;
			ip++;
			break;
		case '2':	// load const
			reg[im]=ir;
			ip++;
			break;
		case '3':	// add const
			reg[im]+=ir;
			reg[im]%=RAMSIZE;
			ip++;
			break;
		case '4':	// multiply const
			reg[im]*=ir;
			reg[im]%=RAMSIZE;
			ip++;
			break;
		case '5':	// load variable
			reg[im]=reg[ir];
			ip++;
			break;
		case '6':	// add variable
			reg[im]+=reg[ir];
			reg[im]%=RAMSIZE;
			ip++;
			break;
		case '7':	// multiply variable
			reg[im]*=reg[ir];
			reg[im]%=RAMSIZE;
			ip++;
			break;
		case '8':	// load from memory
			reg[im]=std::stoi(ram[reg[ir]]);
			ip++;
			break;
		case '9':	// store to memory
			ram[reg[ir]]=std::to_string(reg[im]);
			while(ram[reg[ir]].size()<INSTLEN)
				ram[reg[ir]]="0"+ram[reg[ir]]; // make sure the length of string is fixed
			ip++;
			break;
		default:
			cerr<<inst<<": unknown instruction."<<endl;
			return false;
			break;
	}

	return true;
}

// run all instructions in ram
int SingleInterp::Run() {
	if(ram[ip]!="000") {
		stat=run;
		while(stat==run) { // run all instructions until halt
			if(ExecInst(ram[ip]))
				ic++;
		}
	} else {
		cerr<<"Error: case "<<cn<<" failed to run."<<endl;
		stat=halt;
	}

	return ic;
}

// constructor
Interpreter::Interpreter() {

}

// constructor
Interpreter::Interpreter(string fname) {
	Load(fname);
}

// destructor
Interpreter::~Interpreter() {
	// TBD
}

// read instructions from stdin, and create interpreters for each case respectively.
void Interpreter::Load() {
	vector<string> insts;
	for(string line; getline(std::cin, line);) {
		if(line.size()>0) {
			if(toupper(line[0])=='E')
				break; // finished typing

			int num=std::stoi(line);
			if(num>0 && num<10) { // case number
				cns.push_back(num);
				if(!insts.empty()) { 
					// create a interpreter for previous case
					SingleInterp intp(cns.back(),insts);
					interps.insert(std::pair<int,SingleInterp>(cns.back(),intp));
					insts.clear();
				}
			} else { // add instruction
				if(num<0 || num>=1000)
					cerr<<num<<": invalid instruction - must be an integer in 0~999. "<<endl;
				else
					insts.push_back(line);
			}
		} // end of if
    } // end for

	if(!insts.empty()) { 
		// create a interpreter for previous case
		SingleInterp intp(cns.back(),insts);
		interps.insert(std::pair<int,SingleInterp>(cns.back(),intp));
		insts.clear();
	}
}

// Load instructions from file, and create interpreters for each case respectively.
void Interpreter::Load(string fname) {
	std::fstream fs(fname,std::fstream::in);
	if(fs.is_open()) {
		string line;
		vector<string> insts;
		while(getline(fs,line)) {
			if(line.size()>0) {
				int num=std::stoi(line);
				if(num>0 && num<10) { // case number
					cns.push_back(num);
					if(!insts.empty()) { 
						// create a interpreter for previous case
						SingleInterp intp(cns.back(),insts);
						interps.insert(std::pair<int,SingleInterp>(cns.back(),intp));
						insts.clear();
					}
				} else { // instruction
					insts.push_back(line);
				}
			} // end of if
		} // end while

		// build interpreter for the last case
		if(!insts.empty()) { 
			// create a interpreter for previous case
			SingleInterp intp(cns.back(),insts);
			interps.insert(std::pair<int,SingleInterp>(cns.back(),intp));
			insts.clear();
		}

	} else {
		cerr<<"Failed to open file "<<fname<<"!"<<endl;
	}

	fs.close();
}

// run all interpreters, and return instruction counters
unordered_map<int,int>& Interpreter::Run() {
	for(auto& cn:cns) {
		if(!ValidCN(cn)) {
			cout<<"Error: Case "<<cn<<" failed to run."<<endl;
			continue;
		}
		int ic=interps[cn].Run();
		ics.insert(std::pair<int,int>(cn,ic));
	}

	return ics;
}

// get the instruction counter of the specified case number
int Interpreter::GetIC(int cn) {
	if(!ValidCN(cn)) {
		cout<<"Error: no record found for case "<<cn<<endl;
	} else {
		if(interps[cn].State()!=halt)
			cerr<<"Warning: case "<<cn<<" is still in process."<<endl;
		else
			return interps[cn].GetIC();
	}

	return 0;
}

// get the case numbers
vector<int>& Interpreter::GetCNs() {
	return cns;
}

// write results to stdout
void Interpreter::Output() {
	for(auto& cn:cns) {
		if(!ValidCN(cn)) {
			cout<<"Error: no record found for case "<<cn<<endl;
			continue;
		}

		if(interps[cn].State()!=halt)
			cerr<<"Warning: case "<<cn<<" is still in process."<<endl;
		else
			cout<<"\nCase "<<cn<<": "<<ics[cn]<<" instructions executed."<<endl;
	}
}

// write results to file
void Interpreter::Output(string fname) {
	std::fstream fs(fname,std::fstream::out);

	if(fs.is_open()) {
		for(auto& cn:cns) {
			if(!ValidCN(cn)) {
				fs<<"Error: no record found for case "<<cn<<endl;
				continue;
			}

			if(interps[cn].State()!=halt)
				fs<<"Warning: case "<<cn<<" is still in process."<<endl;
			else
				fs<<"\nCase "<<cn<<": "<<ics[cn]<<" instructions executed."<<endl;
		}
	}

	fs.close();
}

// check if given cn is valid or not
bool Interpreter::ValidCN(int cn) {
	unordered_map<int,SingleInterp>::iterator got=interps.find(cn);
	if(got==interps.end()) {
		return false;
	} else {
		return true;
	}
}
