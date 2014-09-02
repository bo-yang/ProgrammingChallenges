
## Problem

I have a csv format table of telephone calls; each call is on a line of the format “calling #, called #” (e.g., several lines similar to "123456, 222".) Define friends as any two people who have talked, and acquaintances as any two people who are not friends, but share a friend.  Thus, if A talks to B, and C talks to B, then A and B are friends, A and C are acquaintances.  Find who has the most acquaintances, and print out that person's phone number and the count of that persons acquaintances (how many acquaintances). The table will have at most 1,000,000 entries, and the phone numbers will be integers with at most 15 digits.

## Solution

Phone calls are stored in a hash table(HashCall calls). For each entry, the key is
the calling number, and the value is a hash table of called numbers(friends of the
calling number). The reason for storing friends in another hash table is that finding
an element in unordered_map costs constant time on average. 

The acquaitances for each calling number is counted iteratively: given a calling 
number, go through all of its friends' friends. For calling number A, one of A's 
frind B and one of B's frind C, if C not in A's friend list and A is not in C's 
friend list, either, then A and C are acquaintances. All phone numbers and the 
corresponding numbers their acquaintances are stored in a hash table.

## Analysis

Complexity - givn n calls:

The time complexity of reading call log is O(n), finding friendships & acquaintances
costs O(1) on average(but O(n^3) at the worst case, due to C++ unordered_map) and
finding the number with most acquaintances requires O(n). Therefore, on average the
time complexity is O(n).
The space complexity is also O(n), because each number is stored for 3 times at most.

## Build

To build my code, please use command like:

	g++ -std=c++11 -o finder AcquaintsFinder.h AcquaintsFinder_Test.cc

and the usage of my code is:

	Usage: finder -i <calls_log>

