//Structure for cache, used by both L1 and L2 level cache

#include <bits/stdc++.h>
#include "addrStructure.cpp"
#include "mainMemoryStructure.cpp"
using namespace std;

typedef unsigned int u_int;

class cacheStructure {

	int cacheId;		//1: L1 cache  and 2: L2 cache
	u_int blockSize;
	u_int numOfWay;
	u_int numOfSet;
	u_int cacheSize;

	int blockOffsetBits;
	int tagBits;
	int indexBits;

	std::vector<std::vector<long> > tagArray;
	std::vector<std::vector<long> > dataArray;
	std::vector<std::vector<unsigned long long> > lastAccessTime;
	std::vector<std::vector<bool> > validBitArray;
	std::vector<std::vector<bool> > dirtyBitArray;

public:
	//setter functions
	void setTag(u_int way, u_int index, long tagValue);
	void setData(u_int way, u_int index, long data);
	void setLastAccessTime(u_int way, u_int index);
	void setValidBit(u_int way, u_int index, bool validFlag);
	void setDirtyBit(u_int way, u_int index, bool dirtyFlag);

	//getter functions
	int getCacheId();
	u_int getBlockSize();
	u_int getNumOfSet();
	u_int getNumOfWay();
	u_int getCacheSize();
	long getTag(u_int way, u_int index);
	long getData(u_int way, u_int index);
	unsigned long long getLastAccessTime(u_int way, u_int index);
	bool getValidBit(u_int way, u_int index);
	bool getDirtyBit(u_int way, u_int index);

	int getBlockOffsetBits();
	int getTagBits();
	int getIndexBits();

	//member functions to support cache operations
	void init_cache(string fileToRead, int cacheID);
	long readCache(addrStructure address, int way);
	void writeCache(addrStructure address, int way, long data);
	int blockToReplace(addrStructure address);
	int tagComparision(long indexValue, long tagValue);
};