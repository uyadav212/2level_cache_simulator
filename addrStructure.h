//Structure for managing address

#include <bits/stdc++.h>
using namespace std;

#define ADDRSTRUCTURE_H

class addrStructure {

	string addrInBitStr;
	string tagPart;
	string indexPart;
	string offsetPart;

	long tagValue;
	long indexValue;

public:
	string addrInHex;

	//getter functions
	string getAddrInBitStr();
	string getTagPart();
	string getIndexPart();
	string getOffsetPart();
	long getTagValue();
	long getIndexValue();

	//member function to support operations on address
	void addressProcessing(int tagBits, int indexBits, int offsetBits);
};