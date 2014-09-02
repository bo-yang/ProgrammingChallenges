## Problem

I have a file containing a table of integer points in 3-space; each point is on a line of the format

	23 56 78

which represents a point in 3-space with coordinates `x=23, y=56, z=78`.  The file will contain many such lines.

Write a program to open and read the file, and print the points out in order of distance from the first point in the file.  For example, if the first line of the file represents point (12, 5, 13), the points should be printed in order of distance from this point.

Sample input:

	23  56  78
	-3   41  20
	9   19  14
	144 8  -24
	-4  5  77
	4  9  -17

## Solution

The 3D points are stored in a vector, while the distances are stored in another vector together with point indices(`<index,distance>` pair). Euclidean distance is used in my program, and the STL sort function is called to sort the vector of pairs, whose time complexity is O(n*log(n)) on average.

## Build

To build my code, please use command like

	g++ -o psorter -std=c++11 PointsSorter.h PointsSorter_test.cc

where `PointsSorter.h` is the implementation of sorting 3D points and `PointsSorter_test.cc` is the test file.

The usage of the compiled executable is

	Usage: psorter -i <file_3d_points> -o <file_sorted_points> [-p] -[g]
	 where
	  file_3d_points -- input file of 3D points to be sorted
	  file_sorted_points -- output file of the sorted points
	  -p -- flag of printing sorted points to the monitor(false by default), optional
	  -g -- flag of generating random points for test(false by default), optional
