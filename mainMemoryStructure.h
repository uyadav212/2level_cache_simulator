//Structure for Main memory

#include <bits/stdc++.h>
#ifndef ADDRSTRUCTURE_H
	#include "addrStructure.h"
#endif
using namespace std;


class mainMemoryStructure {

	/*****************************************************************************************
	This is just for representation purpose and do not serve functional purpose of main memory
	*****************************************************************************************/
	std::map<string, long> mainMemStorage;

public:
	
	//member functions to support main memory operations for our simulation purpose
	long readFromMM(string address);
	void writeToMM(string address, long data);
};