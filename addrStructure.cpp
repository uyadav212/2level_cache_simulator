//Define address related functions
#include "addrStructure.h"

//getter functions
string addrStructure::getAddrInBitStr() {
	return this->addrInBitStr;
}

string addrStructure::getTagPart() {
	return this->tagPart;
}

string addrStructure::getIndexPart() {
	return this->indexPart;
}

string addrStructure::getOffsetPart() {
	return this->offsetPart;
}

long addrStructure::getTagValue() {
	return this->tagValue;
}

long addrStructure::getIndexValue() {
	return this->indexValue;
}

//member functions
void addrStructure::addressProcessing(int tagBits, int indexBits, int offsetBits) {

	//Flow followed: HEX addr -> Int addr -> Bit string addr -> get tag, index and block offset parts
	stringstream address(this->addrInHex);
	
	u_int intAddr;
	char *ptr;
    address >> std::hex >> intAddr;
    this->addrInBitStr = (bitset<32>(intAddr)).to_string();

    this->tagPart = this->addrInBitStr.substr(0, tagBits);
    this->indexPart = this->addrInBitStr.substr(tagBits, indexBits);
    this->offsetPart = this->addrInBitStr.substr(tagBits + indexBits, offsetBits);

    this->tagValue = strtol(tagPart.c_str(), &ptr, 2);
    this->indexValue = strtol(indexPart.c_str(), &ptr, 2);
}