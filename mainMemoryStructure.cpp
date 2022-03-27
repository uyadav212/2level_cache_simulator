//Define dummy main memory functions
#include "mainMemoryStructure.h"

//member functions to support main memory operations for our simulation purpose
long mainMemoryStructure::readFromMM(string address) {

	//this function mimics the behavior as if it has read from main memory and is returning the data to cache

	std::map<string, long>::iterator it;

	it = mainMemStorage.find(address);
	
	if (it == mainMemStorage.end()) {
		//assuming this random data generated here is present in the main memory
		srand(time(0));
		mainMemStorage[address] = rand();
	}

	return mainMemStorage[address];
}


void mainMemoryStructure::writeToMM(string address, long data) {

	//this function mimics the write operation to the main memory

	//assume writting to main memory........
	mainMemStorage[address] = data;
}