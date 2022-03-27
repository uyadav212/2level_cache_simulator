//Define cache functions
#include <sys/time.h>
#include "cacheStructure.h"

unsigned long long getCurrMicroSec() {
	
	struct timeval timeHolder;

	gettimeofday(&timeHolder, NULL);
	return ((unsigned long long)(timeHolder.tv_sec) * 1000000 + (unsigned long long)(timeHolder.tv_usec));
}

//setter functions
void cacheStructure::setTag(u_int way, u_int index, long tagValue) {
	this->tagArray[way][index] = tagValue;
}

void cacheStructure::setData(u_int way, u_int index, long dataValue) {
	this->dataArray[way][index] = dataValue;
}

void cacheStructure::setLastAccessTime(u_int way, u_int index) {
	this->lastAccessTime[way][index] = getCurrMicroSec();
}


void cacheStructure::setValidBit(u_int way, u_int index, bool vaildBit) {
	this->validBitArray[way][index] = vaildBit;
}

void cacheStructure::setDirtyBit(u_int way, u_int index, bool dirtyFlag) {
	this->dirtyBitArray[way][index] = dirtyFlag;
}

//getter functions
int cacheStructure::getCacheId() {
	return this->cacheId;
}

u_int cacheStructure::getBlockSize() {
	return this->blockSize;
}

u_int cacheStructure::getNumOfSet() {
	return this->numOfSet;
}

u_int cacheStructure::getNumOfWay() {
	return this->numOfWay;
}

u_int cacheStructure::getCacheSize() {
	return this->cacheSize;
}


long cacheStructure::getTag(u_int way, u_int index) {
	return this->tagArray[way][index];
}

long cacheStructure::getData(u_int way, u_int index) {
	return this->dataArray[way][index];
}

unsigned long long cacheStructure::getLastAccessTime(u_int way, u_int index) {
	return this->lastAccessTime[way][index];
}

bool cacheStructure::getValidBit(u_int way, u_int index) {
	return this->validBitArray[way][index];
}

bool cacheStructure::getDirtyBit(u_int way, u_int index) {
	return this->dirtyBitArray[way][index];
}


int cacheStructure::getBlockOffsetBits() {
	return this->blockOffsetBits;
}

int cacheStructure::getTagBits() {
	return this->tagBits;
}

int cacheStructure::getIndexBits() {
	return this->indexBits;
}


//Member functions to support operations
void cacheStructure::init_cache(string fileToRead, int cacheID) {

	ifstream readConfigFile;

    readConfigFile.open(fileToRead);
    if (readConfigFile.is_open()) {

	    readConfigFile >> this->cacheSize;
	    readConfigFile >> this->blockSize;
	    readConfigFile >> this->numOfWay;
	    readConfigFile.close();
    } else {
    	cout << "\n ------Unable to open configure file for cache----------\n\n";
    }

    this->cacheId = cacheID;

    //calculate other parameters
    this->cacheSize *= pow(2,10);				//as it is provided in KB
    this->numOfSet = (this->cacheSize / this->blockSize) / this->numOfWay;

    this->blockOffsetBits = (u_int) log2(this->blockSize);
    this->indexBits = (u_int) log2(this->numOfSet);
    this->tagBits = 32 - this->blockOffsetBits - this->indexBits;

    //define different array sizes
    this->validBitArray.resize(this->numOfWay);
    this->dirtyBitArray.resize(this->numOfWay);
    this->tagArray.resize(this->numOfWay);
    this->dataArray.resize(this->numOfWay);
    this->lastAccessTime.resize(this->numOfWay);

	unsigned long long startTime;
	startTime = getCurrMicroSec();

    for(int it = 0; it < this->numOfWay; it++) {

    	this->validBitArray[it].resize(this->numOfSet, false);
    	this->dirtyBitArray[it].resize(this->numOfSet, false);
    	this->tagArray[it].resize(this->numOfSet);
    	this->dataArray[it].resize(this->numOfSet);
	    this->lastAccessTime[it].resize(this->numOfSet, startTime);
    }
}

int cacheStructure::tagComparision(long indexValue, long tagValue) {

	//returns way number if block found in cache else returns -1
	for (int it = 0; it < this->numOfWay; it++) {

		if (this->tagArray[it][indexValue] == tagValue
			&& this->validBitArray[it][indexValue] == true) {
			return it;
		}
	}

	//when it is a miss
	return -1;
}

long cacheStructure::readCache(addrStructure address, int way) {

	//reads the desired block from cache and returns the data
	if (this->validBitArray[way][address.getIndexValue()] == true) {

		this->lastAccessTime[way][address.getIndexValue()] = getCurrMicroSec();
		return	this->dataArray[way][address.getIndexValue()];
	}
	else
		return LONG_MIN;
}

void cacheStructure::writeCache(addrStructure address, int way, long data) {

	//this function implements write-back approach, hence unless it is to be replace it will not write data to main memory
	if (this->validBitArray[way][address.getIndexValue()] == true) {
		
		this->dataArray[way][address.getIndexValue()] = data;
		this->dirtyBitArray[way][address.getIndexValue()] = true;
		this->lastAccessTime[way][address.getIndexValue()] = getCurrMicroSec();
	}
}

int cacheStructure::blockToReplace(addrStructure address) {

	//this function looks for block to replace in the cache based on LRU replacement policy
	int toReplace = -1;
	unsigned long long minTime = getCurrMicroSec();

	for (int it = 0; it < this->numOfWay; it++) {

		if (this->validBitArray[it][address.getIndexValue()] == false) {
			
			//found an empty block, so we can use this block
			toReplace = it;
			break;
		} else if (minTime >= this->lastAccessTime[it][address.getIndexValue()]) {
			toReplace = it;
		}
	}
	return toReplace;
}
