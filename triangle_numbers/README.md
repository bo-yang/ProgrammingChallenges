## Problem

An example of triangle numbers is:

	      1
	     / \
	    2   5
	   / \ / \ 
	  9   4  33
	 / \ / \/  \ 
	11  6  99   0

In each row (except for the last), the numbers are adjacent to two numbers on the row below it. For example, 9 is adjacent to 11 and 6.

Imagine drawing a path connecting adjacent numbers from top to bottom. Take the sum of the numbers in this path. The lowest possible sum you could arrive at for this triangle is 13 (1 + 2 + 4 + 6).

Write a program which will read a file describing a triangle such as this one, and determine the lowest possible sum of numbers which connect the top and the bottom. The input will be a tab-separated list of numbers.

## Solution

The lowest sum can be found recursively by choosing the child node with the smallest sum. This greedy strategy should work for both positive and negative numbers, because every possible combination of nearby numbers are considered in the recursion. 

The recursion function is:

	OPT(i,j)=tnum(i,j)+min(OPT(i+1,j),OPT(i+1,j+1))

where `OPT(i,j)` is the lowest sum of number `tnum[i][j]`. Above function can be implemented by dynamic programming.

Given a triangle number with n lines, then:

 - Time complexity for finding the lowest sum of numbers: `O(n^2)`.
 - Time complexity for finding the shortest path: `O(n)`.

To build the executable file, please use command

	g++ -o question2 question2.h question2_test.cc

To test my implementation, you can either

- specify a file of triangle numbers by option `-f <file>`, or
- generate random triangle numbers by options 
	`-l <#lines_triangle_tobe_generated> -m <maximum_numbers> -n <flag_allow_negative_numbers>`. 
