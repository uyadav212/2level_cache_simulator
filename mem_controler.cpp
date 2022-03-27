//This is to simulate memory controller
#include "cacheStructure.cpp"

cacheStructure l1Cache;
cacheStructure l2Cache;
mainMemoryStructure mainMemory;

// function to convert decimal to binary
string decimalToBinary(int decimal, int numOfBits) {
    
    string binaryBits;
    string binaryNum;
    
    for (int it = 0; decimal > 0; it++) {
 
        (decimal % 2) ? binaryBits.push_back('1') : binaryBits.push_back('0');
        decimal /= 2;
    }
 
    // accessing binary bits in reverse order
    while (numOfBits > binaryBits.length()) {
    	binaryNum.push_back('0');
    	numOfBits--;
    }

    for (int it = binaryBits.length() - 1;it >= 0; it--)
        binaryNum.push_back(binaryBits.at(it));

    return binaryNum;
}

void performReplaceBlock(int toReplace, addrStructure address, cacheStructure *cache, long data) {

	//check if block we are replacing needs to be written to main memory before eviction [as cache is write-back]
	if (cache->getValidBit(toReplace, address.getIndexValue() == true)
		&& cache->getDirtyBit(toReplace, address.getIndexValue()) == true) {

		mainMemory.writeToMM(address.getAddrInBitStr(), cache->getData(toReplace, address.getIndexValue()));

		//also in case lower level [L2] is having this block, then invalidate it to keep all the images in sync
		if (cache->getCacheId() == 1) {
			
			string tagAndIndexBits = decimalToBinary(cache->getTag(toReplace, address.getIndexValue()), cache->getTagBits())
								+ decimalToBinary(address.getIndexValue(), cache->getIndexBits());

			long l2TagForBlockToReplace, l2IndexForBlockToReplace;
			char *ptr;

			l2TagForBlockToReplace = strtol((tagAndIndexBits.substr(0, l2Cache.getTagBits())).c_str(), &ptr, 2);
			l2IndexForBlockToReplace = strtol((tagAndIndexBits.substr(l2Cache.getTagBits())).c_str(), &ptr, 2);

			//check if this block is present in L2 cache
            int wayHit = l2Cache.tagComparision(l2IndexForBlockToReplace, l2TagForBlockToReplace);
            if (wayHit != -1)
            	l2Cache.setValidBit(wayHit, l2IndexForBlockToReplace, false);
		}
	}

	cache->setData(toReplace, address.getIndexValue(), data);
	cache->setTag(toReplace, address.getIndexValue(), address.getTagValue());
	cache->setValidBit(toReplace, address.getIndexValue(), true);
	cache->setDirtyBit(toReplace, address.getIndexValue(), false);
	cache->setLastAccessTime(toReplace, address.getIndexValue());
}

int main(int countArg, char* valuesArg[]) {

	//L1 cache
	l1Cache.init_cache("configureL1Cache.txt", 1);
	
	//L2 cache
	l2Cache.init_cache("configureL2Cache.txt", 2);

	long l1Hit, l1Miss, l2Hit, l2Miss, numOfRequests;
	l1Hit = l1Miss = l2Hit = l2Miss = numOfRequests = 0;

	ifstream cacheRequest;
	ofstream cacheResponse;

	cacheRequest.open("CacheRequest.txt");
	cacheResponse.open("CacheResponse.txt");

	if (cacheResponse.is_open() && cacheRequest.is_open()) {

		string requestTrace;
		string accessType;
		string ignore;
		addrStructure addressL1;
		long dummyWriteData = 1;		//this data written in case of write operation is just for simulation purpose
		int wayHit = -1;

		while (getline(cacheRequest, requestTrace)) {

			cout << endl << numOfRequests << " request processing: " << requestTrace;
			numOfRequests++;

			istringstream getInputsFromTrace(requestTrace);
            if (!(getInputsFromTrace >> ignore >> accessType >> addressL1.addrInHex))
            	break;

            addressL1.addressProcessing(l1Cache.getTagBits(), l1Cache.getIndexBits(), l1Cache.getBlockOffsetBits());

            //check if required block is available in L1 cache or not (Hit/Miss)
            wayHit = l1Cache.tagComparision(addressL1.getIndexValue(), addressL1.getTagValue());

            if (wayHit == -1) {

            	//It is a miss at L1 cache, proceed accordingly
            	cacheResponse << "L1_Miss\t";
            	l1Miss++;

            	//go to next level (L2 cache) and search for required block 
				addrStructure addressL2;
            	addressL2.addressProcessing(l2Cache.getTagBits(), l2Cache.getIndexBits(), l2Cache.getBlockOffsetBits());

	            //check if required block is available in L2 cache or not (Hit/Miss)
	            wayHit = l2Cache.tagComparision(addressL2.getIndexValue(), addressL2.getTagValue());

	            if (wayHit == -1) {

	            	//It is a miss at L2 cache, proceed accordingly
	            	cacheResponse << "L2_Miss\t";
	            	l2Miss++;

	            	if (!strcmp(accessType.c_str(), "R")) {

	            		//bring block to L2 cache and L1 cache from main memory
	            		long dataFromMM;
	            		int wayReplaced;

	            		dataFromMM = mainMemory.readFromMM(addressL1.getAddrInBitStr());
	            		wayReplaced = l2Cache.blockToReplace(addressL2);
	            		performReplaceBlock(wayReplaced, addressL2, &l2Cache, dataFromMM);
	            		wayReplaced = l1Cache.blockToReplace(addressL1);
	            		performReplaceBlock(wayReplaced, addressL1, &l1Cache, dataFromMM);

	         			cacheResponse << "\t" << l1Cache.readCache(addressL1, wayReplaced) << " [R from MM]\t";
	            	}
	         		else if (!strcmp(accessType.c_str(), "W")) {

	         			mainMemory.writeToMM(addressL1.getAddrInBitStr(), dummyWriteData);
		            	cacheResponse << "\t" << dummyWriteData << " [W to MM]\t";
	         		}
	         		else
	         			cacheResponse << "[Unknown access type]";

		            cacheResponse << "\t[Main Memory]\n";
	            } else {

	            	//It is a hit at L2 cache, proceed with read or write
	            	cacheResponse << "L2_Hit\t";
	            	l2Hit++;

	            	if (!strcmp(accessType.c_str(), "R")) {

	            		//As it is a read hit on L2, we will bring the required block from L2 to L1 cache
	            		long dataFromL2 = l2Cache.readCache(addressL2, wayHit);
	            		int wayReplaced = l1Cache.blockToReplace(addressL1);
	            		performReplaceBlock(wayReplaced, addressL1, &l1Cache, dataFromL2);

	            		//also check if block present in L2 is clean or dirty, if dirty then set dirty bit in L1 too
	            		l1Cache.setDirtyBit(wayReplaced, addressL1.getIndexValue(), l2Cache.getDirtyBit(wayHit, addressL2.getIndexValue()));

	         			cacheResponse << "\t" << l1Cache.readCache(addressL1, wayReplaced) << "[R from L2]\t";
	            	}
	         		else if (!strcmp(accessType.c_str(), "W")) {

	         			l2Cache.writeCache(addressL2, wayHit, dummyWriteData);
		            	cacheResponse << "\t" << dummyWriteData << " [W to L2]\t";
	         		}
	         		else
	         			cacheResponse << "[Unknown access type]";

		            cacheResponse << "\t[Tag: " << addressL2.getTagValue() << ", Index: " << addressL2.getIndexValue() << "]\n";
	            }
            } else {

            	//It is a hit at L1 cache, proceed with read or write
            	cacheResponse << "L1_Hit\t";
            	l1Hit++;
            	
            	if (!strcmp(accessType.c_str(), "R")) {

         			cacheResponse << "\t" << l1Cache.readCache(addressL1, wayHit) << " [R from L1]\t";
            	}
         		else if (!strcmp(accessType.c_str(), "W")) {

         			l1Cache.writeCache(addressL1, wayHit, dummyWriteData);
	            	cacheResponse << "\t" << dummyWriteData << " [W to L1]\t";
         		}
         		else
         			cacheResponse << "[Unknown access type]";
	            
	            cacheResponse << "\t[Tag: " << addressL1.getTagValue() << ", Index: " << addressL1.getIndexValue() << "]\n";
            }
			dummyWriteData++;		//creating dummy data to write
		}

		//printing stats
		float l1HitRatio, l1MissRatio, l2HitRatio, l2MissRatio;
		l1HitRatio = (float)l1Hit/(float)numOfRequests*100;
		l1MissRatio = (float)l1Miss/(float)numOfRequests*100;
		l2HitRatio = (float)l2Hit/(float)l1Miss*100;
		l2MissRatio = (float)l2Miss/(float)l1Miss*100;

		cout << "\n\n======================================================\n";
		cout << "Total number of CPU requests: " << numOfRequests << endl;
		cout << "L1 Cache =>\tHit: " << l1Hit << "[" << l1HitRatio << "%]" << "\tMiss: " << l1Miss << "[" << l1MissRatio << "%]" << endl;
		cout << "L2 Cache =>\tHit: " << l2Hit << "[" << l2HitRatio << "%]" << "\tMiss: " << l2Miss << "[" << l2MissRatio << "%]" << endl;
		cout << "======================================================\n";

		//Print same stats to cacheResponse file too [in the end]
		cacheResponse << "\n\n======================================================\n";
		cacheResponse << "Total number of CPU requests: " << numOfRequests << endl;
		cacheResponse << "L1 Cache =>\tHit: " << l1Hit << "[" << l1HitRatio << "%]" << "\tMiss: " << l1Miss << "[" << l1MissRatio << "%]" << endl;
		cacheResponse << "L2 Cache =>\tHit: " << l2Hit << "[" << l2HitRatio << "%]" << "\tMiss: " << l2Miss << "[" << l2MissRatio << "%]" << endl;
		cacheResponse << "======================================================\n";

		cacheResponse.close();
		cacheRequest.close();
	} else {

		cout << "\n------Unable to open input/output trace files-----------\n\n";
	}

	return 0;
}