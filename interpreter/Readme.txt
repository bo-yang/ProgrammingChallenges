1. Files
This is the C++ implementation of an interpreter, which contains following
files:
	- interpreter.h, 	defines two classes SingleInterp and Interpreter.
	- interpreter.cc, 	implementation of class functions.
	- main.cc, 			test file.
	- input.txt, 		input instructions
	- output.txt,		output of my code.

2. Details of my implementation
Class SingleInterp defines an interpreter for one case, while class Interpreter
support multiple cases, each of which has a separate SingleInterp repectively.

3. Commands to build
You can use following command to compile my codes:

	g++ -std=c++11 -o interp interpreter.h interpreter.cc main.cc

Since C++11 is used, you need a compiler that supports C++11, such as GCC4.7+.

To test my code, you can use command
	./interp -i input.txt -o output.txt
or
	./interp

The former command would load and run instructions stored in input.txt and
write the results to output.txt. The latter one would ask you to enter
instructions via command line.

I could sucessfully run my code with GCC4.9 on Mac OS X.
