#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include <cctype>

using std::endl;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
using std::unordered_map;

namespace interp {
	const int RAMSIZE=1000;	// memory size
	const int REGSIZE=10;	// register size
	const int INSTLEN=3;	// length of instructions
	enum IntpState {init,run,halt};

	// Interpreter class of one case
	class SingleInterp {
	public:
		SingleInterp();
		SingleInterp(int case_no,vector<string>& insts);
		~SingleInterp();
	
		bool Load(vector<string>& insts); // load instructions
		//bool InsertInst(string inst); // insert an instruction into the memory
		bool ExecInst(string inst);	// execute an instrution
		int Run();				// run all instructions in ram
		int GetIC();			// get the instruction counter
		int GetCN();			// return the case number
		IntpState State();	// get the status of the interpreter

	private:
		vector<string> ram;	// memory
		vector<int> reg;	// register
		int ic;		// instruction counter
		int ip;		// instruction pointer
		int cn;		// case number
		IntpState stat;		// status
	};
	
	// Interpreter class
	class Interpreter {
	public:
		Interpreter();
		Interpreter(string fname);
		~Interpreter();
	
		void Load(string fname); // load instructions from file
		void Load();			// read instructions from stdin
		unordered_map<int,int>& Run();		// run all interpreters, return instruction counters
		int GetIC(int cn);		// get the instruction counter of the specified case number
		vector<int>& GetCNs();	// get the case numbers
		void Output(); // write results to stdout
		void Output(string fname); // write results to file
		bool ValidCN(int cn);	// check if cn is valid or not
	
	private:
		unordered_map<int,SingleInterp> interps; // interpraters, <case_number, interprater> map
		unordered_map<int,int> ics;	// instruction counters, <case_number, instruction_counter> map
		vector<int> cns;			// case numbers
	};
}

#endif /* INTERPRETER_H */
