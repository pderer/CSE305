#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

using namespace std;

random_device rd;
mt19937 mersenne(rd());

struct BlockLRU {
	int64_t data, addr;
	bool dirty;
	BlockLRU* prev;
	BlockLRU* next;

	BlockLRU(){
		next = prev = NULL;
		data = 0;
		addr = 0;
		dirty = false;
	}
	BlockLRU(int64_t i, int64_t ad){
		next = prev = NULL;
		data = i;
		addr = ad;
		dirty = false;
	}
};

struct ListLRU {
	BlockLRU* head;  // delete after
	BlockLRU* tail;  // delete after
	int size, dirtyEvict, cleanEvict;
	int writeHit, writeMiss, readHit, readMiss;
	
	ListLRU(int i){
		size = i;
		head = new BlockLRU();
		tail = new BlockLRU();
		head->next = tail;
		tail->prev = head;
		head->data = -1;
		tail->data = -1;
		dirtyEvict = 0;
		cleanEvict = 0;
		writeHit = 0;
		writeMiss = 0;
		readHit = 0;
		readMiss = 0;
	}
	BlockLRU* LRUSearch(int64_t tag){
		BlockLRU* temp = head->next;
		while(temp->data > 0){
			if(temp->data == tag){
				return temp;
			}
			temp = temp->next;
		}
		return NULL;
	}
	void LRUHit(BlockLRU* temp, int64_t tag, int64_t addr, string RW, int level){
		rmBlock(temp);
		addBlock(tag, addr, RW, level);
		if(RW == "R"){readHit++;}
		else{writeHit++;}
	}
	void rmBlock(BlockLRU* temp){
		BlockLRU* delTemp = temp;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		delete delTemp;
	}
	void addBlock(int64_t tag, int64_t addr, string RW, int level){
		BlockLRU* newBlock = new BlockLRU(tag, addr);
		head->next->prev = newBlock;
		newBlock->next = head->next;
		head->next = newBlock;
		newBlock->prev = head;
		if(level == 1){
			if(RW == "W"){
				newBlock->dirty = true;
			}
		}
	}
	int64_t LRUMiss(int64_t tag, int64_t addr, string RW, int level){
		addBlock(tag, addr, RW, level);
		int64_t tmp = -1;
		if(listLen() > size){
			tmp = rmTail(RW);
		}
		if(RW == "R"){readMiss++;}
		else{writeMiss++;}
		return tmp;
	}
	int listLen(){
		int result = 0;
		BlockLRU* tmp = head->next;
		while(tmp->data > 0){
			result += 1;
			tmp = tmp->next;
		}
		return result;
	}
	int64_t rmTail(string RW){
		int64_t tmp = 0;
		BlockLRU* delTemp = tail->prev;
		tmp = delTemp->addr;
		tail->prev->prev->next = tail;
		tail->prev = tail->prev->prev;
		if(tail->prev->dirty){
			dirtyEvict++;
		} else {
			cleanEvict++;
		}
		delete delTemp;
		return tmp;
	}
	void makeDirty(BlockLRU* tmp){
		tmp->dirty = true;
	}
	bool rmL1Block(BlockLRU* temp){
		BlockLRU* delTemp = temp;
		bool isDirty;
		if(temp->dirty){
			dirtyEvict++;
			isDirty = true;
		} else {
			cleanEvict++;
			isDirty = false;
		}
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		delete delTemp;
		return isDirty;
	}
	void minusClean(){
		cleanEvict -= 1;
	}
	void plusDirty(){
		dirtyEvict += 1;
	}
};

struct BlockRan {
	int64_t data, addr;
	bool dirty;
	bool valid;

	BlockRan(){
		data = 0;
		addr = 0;
		dirty = false;
		valid = false;
	}
	BlockRan(int64_t i, int64_t ad){
		data = i;
		addr = ad;
		dirty = false;
		valid = false;
	}
};

struct ListRan {
	BlockRan* way;
	int size, dirtyEvict, cleanEvict;
	int writeHit, writeMiss, readHit, readMiss;

	ListRan(int i){
		size = i;
		way = new BlockRan[i];
		dirtyEvict = 0;
		cleanEvict = 0;
		writeHit = 0;
		writeMiss = 0;
		readHit = 0;
		readMiss = 0;
	}
	int RanSearch(int64_t tag){
		for(int i = 0; i < size; i++){
			if(way[i].data == tag && way[i].valid == true){
				return i;
			}
		}
		return -1;
	}
	void RanHit(int index, string RW, int level){
		if(RW == "R"){readHit++;}
		else {
			writeHit++;
			if(level == 1){
				way[index].dirty = true;
			}
		}
	}
	int64_t RanMiss(int64_t tag, int64_t addr, string RW, int level){
		int64_t tmp = -1;
		if(listLen() < size){
			for(int i = 0; i < size; i++){
				if(way[i].valid == false){
					way[i].data = tag;
					way[i].addr = addr;
					way[i].valid = true;
					if(RW == "W"){
						if(level == 1){
							way[i].dirty = true;
						}
						else{way[i].dirty = false;}
					}
					else{way[i].dirty = false;}
					break;
				}
			}
		} else { // list full
			uniform_int_distribution<> rand(0, size-1);
			int randIdx = rand(mersenne);
			tmp = way[randIdx].addr;
			way[randIdx].data = tag;
			way[randIdx].addr = addr;
			if(way[randIdx].dirty){dirtyEvict++;}
			else{cleanEvict++;}
			if(RW == "W"){
				if(level == 1){
					way[randIdx].dirty = true;
				}
				else{way[randIdx].dirty = false;}
			}
			else{way[randIdx].dirty = false;}
		}
		if(RW == "R"){readMiss++;}
		else{writeMiss++;}
		return tmp;
	}
	int listLen(){
		int result = 0;
		for(int i = 0; i < size; i++){
			if(way[i].valid){
				result++;
			}
		}
		return result;
	}
	void makeDirty(int index){
		way[index].dirty = true;
	}
	bool rmL1Block(int index){
		bool isDirty;
		if(way[index].dirty){
			dirtyEvict++;
			isDirty = true;
		} else {
			cleanEvict++;
			isDirty = false;
		}
		way[index].valid = false;
		return isDirty;
	}
	void minusClean(){
		cleanEvict -= 1;
	}
	void plusDirty(){
		dirtyEvict += 1;
	}

};

int main(int argc, char* argv[]){
	char c[3] = "-c";
	char a[3] = "-a";
	char b[3] = "-b";
	char l[5] = "-lru";
	char ran[8] = "-random";

	int capacity = 0;
	int assoc = 0;
	int blockSize = 0;
	bool lru = false;
	bool random = false;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], c) == 0){
			capacity = atoi(argv[i+1]);
		}
		else if(strcmp(argv[i], a) == 0){
			assoc = atoi(argv[i+1]);
		}
		else if(strcmp(argv[i], b) == 0){
			blockSize = atoi(argv[i+1]);
		}
		else if(strcmp(argv[i], l) == 0){
			lru = true;
		}
		else if(strcmp(argv[i], ran) == 0){
			random = true;
		}
	}
	
	if(capacity <= 0 || assoc <= 0 || blockSize <= 0 || (lru == false && random == false)){
		cerr << "please enter appropriate parameter option" << endl;
		return 0;
	}

	string fileName(argv[argc-1]);
	ifstream ifas(fileName);
	
	if(!ifas){
		cerr << "could not open this file" << endl;
		return 0;
	}
	
	// prepare set Index and Associativity for L1 and L2

	int L1Index, L2Index, L1Assoc;
	L2Index = capacity*1024/blockSize/assoc;
	if(assoc < 3){
		L1Index = L2Index/4;
		L1Assoc = assoc;
	} else {
		L1Index = L2Index;
		L1Assoc = assoc/4;
	}

	ListLRU* L2LRU[L2Index];  // need to delete after
	ListLRU* L1LRU[L1Index];  // need to delete after
	ListRan* L2Ran[L2Index]; // for random, need to delete after
	ListRan* L1Ran[L1Index]; // for random, need to delete after

	if(lru){
		for(int i = 0; i < L2Index; i++){
			L2LRU[i] = new ListLRU(assoc);
		}
		for(int i = 0; i < L1Index; i++){
			L1LRU[i] = new ListLRU(L1Assoc);
		}
	} else { // random
		for(int i = 0; i < L2Index; i++){
			L2Ran[i] = new ListRan(assoc);
		}
		for(int i = 0; i < L1Index; i++){
			L1Ran[i] = new ListRan(L1Assoc);
		}
	}

	int test;
	int totalAcc = 0;
	int readAcc = 0;
	int writeAcc = 0;
	string line, RW;
	if(lru){ // lru replacement
		cout<<"LRU Caching Process... Maybe it takes few minutes."<<endl;
		while(getline(ifas, line)) {
			RW = line.substr(0, line.find(" "));
			line = line.substr(line.find(" ") + 1, line.length());
			stringstream ss;
			int64_t addr;
			ss << hex << line;
			ss >> addr;
			int blockOffset = (int)log2((float)blockSize);
			int L2IndexBit = (int)log2((float)L2Index);
			int L1IndexBit = (int)log2((float)L1Index);
			int64_t L1Tag = (addr >> (blockOffset + L1IndexBit));
			int64_t L2Tag = (addr >> (blockOffset + L2IndexBit));
			int L1Set = (addr >> blockOffset) - (L1Tag << L1IndexBit);
			int L2Set = (addr >> blockOffset) - (L2Tag << L2IndexBit);
			BlockLRU* L1tmp = L1LRU[L1Set]->LRUSearch(L1Tag);
			BlockLRU* L2tmp = L2LRU[L2Set]->LRUSearch(L2Tag);
			if(L1tmp){  // L1 hit
				L1LRU[L1Set]->LRUHit(L1tmp, L1Tag, addr, RW, 1);
			} else { // L1 Miss
				if(L2tmp){ // L1 Miss, L2 Hit
					L2LRU[L2Set]->LRUHit(L2tmp, L2Tag, addr, RW, 2);
					int dirtytmp = L1LRU[L1Set]->dirtyEvict;
					int64_t addrtmp = L1LRU[L1Set]->LRUMiss(L1Tag, addr, RW, 1); // problem
					if(L1LRU[L1Set]->dirtyEvict > dirtytmp){ // L1 dirty eviction, write L2, careful with write access
						int64_t tegtmp = (addrtmp >> (blockOffset + L2IndexBit));
						int settmp = (addrtmp >> blockOffset) - (tegtmp << L2IndexBit);
						BlockLRU* L2tmp2 = L2LRU[settmp]->LRUSearch(tegtmp);
						if(L2tmp2){
							L2LRU[settmp]->makeDirty(L2tmp2);
						}
					}
				} else { // L1 Miss, L2 Miss
					int64_t addrtmp = L2LRU[L2Set]->LRUMiss(L2Tag, addr, RW, 2);  // check if victim in L1 too.
					if(addrtmp > 0){  // L2 miss and need victim
						int64_t tegtmp = (addrtmp >> (blockOffset + L1IndexBit));
						int settmp = (addrtmp >> blockOffset) - (tegtmp << L1IndexBit);
						BlockLRU* L1tmp2 = L1LRU[settmp]->LRUSearch(tegtmp);
						if(L1tmp2){  // same block in L1 , careful with write access, dirty eviction
							if(L1LRU[settmp]->rmL1Block(L1tmp2)){  // dirty eviction
								L2LRU[L2Set]->minusClean();
								L2LRU[L2Set]->plusDirty();
							}
						}
					}
					int dirtytmp = L1LRU[L1Set]->dirtyEvict;
					int64_t addrtmp2 = L1LRU[L1Set]->LRUMiss(L1Tag, addr, RW, 1);
					if(L1LRU[L1Set]->dirtyEvict > dirtytmp){
						int64_t tegtmp = (addrtmp2 >> (blockOffset + L2IndexBit));
						int settmp = (addrtmp2 >> blockOffset) - (tegtmp << L2IndexBit);
						BlockLRU* L2tmp2 = L2LRU[settmp]->LRUSearch(tegtmp);
						if(L2tmp2){
							L2LRU[settmp]->makeDirty(L2tmp2);
						}
					}	
				}
			}
			totalAcc++;
			if(RW == "R"){readAcc++;}
			else{writeAcc++;}
		}	
	}
	else if(random) { // random replacement
		cout << "Random Caching Process... Maybe it takes few minutes." << endl;
		while(getline(ifas, line)) {
			RW = line.substr(0, line.find(" "));
			line = line.substr(line.find(" ") + 1, line.length());
			stringstream ss;
			int64_t addr;
			ss << hex << line;
			ss >> addr;
			int blockOffset = (int)log2((float)blockSize);
			int L2IndexBit = (int)log2((float)L2Index);
			int L1IndexBit = (int)log2((float)L1Index);
			int64_t L1Tag = (addr >> (blockOffset + L1IndexBit));
			int64_t L2Tag = (addr >> (blockOffset + L2IndexBit));
			int L1Set = (addr >> blockOffset) - (L1Tag << L1IndexBit);
			int L2Set = (addr >> blockOffset) - (L2Tag << L2IndexBit);
			int L1tmp = L1Ran[L1Set]->RanSearch(L1Tag);
			int L2tmp = L2Ran[L2Set]->RanSearch(L2Tag);
			
			if(L1tmp > -1){ // L1 hit
				L1Ran[L1Set]->RanHit(L1tmp, RW, 1);
			} else {  // L1 Miss
				if(L2tmp > -1){ // L1 Miss, L2 Hit
					L2Ran[L2Set]->RanHit(L2tmp, RW, 2);
					int dirtytmp = L1Ran[L1Set]->dirtyEvict;
					int64_t addrtmp = L1Ran[L1Set]->RanMiss(L1Tag, addr, RW, 1);
					if(L1Ran[L1Set]->dirtyEvict > dirtytmp){
						int64_t tegtmp = (addrtmp >> (blockOffset + L2IndexBit));
						int settmp = (addrtmp >> blockOffset) - (tegtmp << L2IndexBit);
						int L2tmp2 = L2Ran[settmp]->RanSearch(tegtmp);
						if(L2tmp2 > -1){
							L2Ran[settmp]->makeDirty(L2tmp2);	
						}
					}
				} else { // L1 Miss, L2 Miss
					int64_t addrtmp = L2Ran[L2Set]->RanMiss(L2Tag, addr, RW, 2);
					if(addrtmp > -1){
						int64_t tegtmp = (addrtmp >> (blockOffset + L1IndexBit));
						int settmp = (addrtmp >> blockOffset) - (tegtmp << L1IndexBit);
						int L1tmp2 = L1Ran[settmp]->RanSearch(tegtmp);
						if(L1tmp2 > -1){
							if(L1Ran[settmp]->rmL1Block(L1tmp2)){
								L2Ran[L2Set]->minusClean();
								L2Ran[L2Set]->plusDirty();
							}
						}
					}
					int dirtytmp = L1Ran[L1Set]->dirtyEvict;
					int64_t addrtmp2 = L1Ran[L1Set]->RanMiss(L1Tag, addr, RW, 1);
					if(L1Ran[L1Set]->dirtyEvict > dirtytmp){
						int64_t tegtmp = (addrtmp2 >> (blockOffset + L2IndexBit));
						int settmp = (addrtmp2 >> blockOffset) - (tegtmp << L2IndexBit);
						int L2tmp2 = L2Ran[settmp]->RanSearch(tegtmp);
						if(L2tmp2 > -1){
							L2Ran[settmp]->makeDirty(L2tmp2);
						}
					}
				}
			}
			totalAcc++;
			if(RW == "R"){readAcc++;}
			else{writeAcc++;}
		}
	}

	ifas.close();

	int L1ReadHit = 0, L1ReadMiss = 0, L1WriteHit = 0, L1WriteMiss=0, L2ReadHit=0, L2ReadMiss=0, L2WriteHit=0, L2WriteMiss=0;
	int L1Clean=0, L1Dirty=0, L2Clean=0, L2Dirty=0;
	if(lru){
		for(int i = 0; i < L2Index; i++){
			L2ReadHit += L2LRU[i]->readHit;
			L2ReadMiss += L2LRU[i]->readMiss;
			L2WriteHit += L2LRU[i]->writeHit;
			L2WriteMiss += L2LRU[i]->writeMiss;
			L2Clean += L2LRU[i]->cleanEvict;
			L2Dirty += L2LRU[i]->dirtyEvict;
			delete L2LRU[i];
		}
		for(int i = 0; i < L1Index; i++){
			L1ReadHit += L1LRU[i]->readHit;
			L1ReadMiss += L1LRU[i]->readMiss;
			L1WriteHit += L1LRU[i]->writeHit;
			L1WriteMiss += L1LRU[i]->writeMiss;
			L1Clean += L1LRU[i]->cleanEvict;
			L1Dirty += L1LRU[i]->dirtyEvict;
			delete L1LRU[i];
		}
	}
	else if(random){ // random
		for(int i = 0; i < L2Index; i++){
			L2ReadHit += L2Ran[i]->readHit;
			L2ReadMiss += L2Ran[i]->readMiss;
			L2WriteHit += L2Ran[i]->writeHit;
			L2WriteMiss += L2Ran[i]->writeMiss;
			L2Clean += L2Ran[i]->cleanEvict;
			L2Dirty += L2Ran[i]->dirtyEvict;
			delete L2Ran[i];
		}
		for(int i = 0; i < L1Index; i++){
			L1ReadHit += L1Ran[i]->readHit;
			L1ReadMiss += L1Ran[i]->readMiss;
			L1WriteHit += L1Ran[i]->writeHit;
			L1WriteMiss += L1Ran[i]->writeMiss;
			L1Clean += L1Ran[i]->cleanEvict;
			L1Dirty += L1Ran[i]->dirtyEvict;
			delete L1Ran[i];
		}
	}
	
	int L1ReadAcc = L1ReadHit + L1ReadMiss;
	int L1WriteAcc = L1WriteHit + L1WriteMiss;
	int L2ReadAcc = L2ReadHit + L2ReadMiss;
	int L2WriteAcc = L2WriteHit + L2WriteMiss;

	float L1ReadMissRate = (float)L1ReadMiss / (float)L1ReadAcc * 100;
	float L1WriteMissRate = (float)L1WriteMiss / (float)L1WriteAcc * 100;
	float L2ReadMissRate = (float)L2ReadMiss / (float)L2ReadAcc * 100;
	float L2WriteMissRate = (float)L2WriteMiss / (float)L2WriteAcc * 100;

	L1ReadMissRate = round(L1ReadMissRate*100)/100;
	L1WriteMissRate = round(L1WriteMissRate*100)/100;
	L2ReadMissRate = round(L2ReadMissRate*100)/100;
	L2WriteMissRate = round(L2WriteMissRate*100)/100;

	// output part
	fileName = fileName.substr(0, fileName.find(".out"));
	string output = fileName + "_" + to_string(capacity) + "_" + to_string(assoc) + "_" + to_string(blockSize) + "_out";
	
	ofstream ofas(output);
	if(ofas.is_open()){
		ofas << "-- General Stats --" << endl
		<< "L1 Capacity: " << capacity/4 << endl
		<< "L1 way: " << L1Assoc << endl
		<< "L2 Capacity: " << capacity << endl
		<< "L2 way: " << assoc << endl
		<< "Block size: " << blockSize << endl
		<< "Total accesses: " << totalAcc << endl
		<< "Read accesses: " << readAcc << endl
		<< "Write accesses: " << writeAcc << endl
		<< "L1 Read misses: " << L1ReadMiss << endl
		<< "L2 Read misses: " << L2ReadMiss << endl
		<< "L1 Write misses: " << L1WriteMiss << endl
		<< "L2 Write misses: " << L2WriteMiss << endl
		<< "L1 Read miss rate: " << L1ReadMissRate << "%" << endl
		<< "L2 Read miss rate: " << L2ReadMissRate << "%" << endl
		<< "L1 Write miss rate: " << L1WriteMissRate << "%" << endl
		<< "L2 Write miss rate: " << L2WriteMissRate << "%" << endl
		<< "L1 Clean eviction: " << L1Clean << endl
		<< "L2 Clean eviction: " << L2Clean << endl
		<< "L1 Dirty eviction: " << L1Dirty << endl
		<< "L2 Dirty eviction: " << L2Dirty << endl;
	} else {
		cerr << "output file error" << endl;
		return 0;
	}
	ofas.close();
	return 0;
}
