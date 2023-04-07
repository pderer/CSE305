#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>

using namespace std;

unsigned int textAddr = 4194304;
unsigned int dataAddr = 268435456;

unsigned int currPc = 4194304;

const char* colon = ":";

void printOutput(unsigned int currPc, map<unsigned int, unsigned int> regs,vector<unsigned int> addrs, map<unsigned int, unsigned int> address){
	cout << "Current register values:" <<endl;
	cout << "------------------------------------" << endl;
	cout << "PC: " <<"0x"<< hex << currPc << endl;
	cout << "Registers:" << endl;
	for(int i = 0; i < 32; i++){
		cout << "R"<< dec << i << ": "<<"0x"<< hex << regs[i] << endl;
	}
	cout<<endl;
	if(addrs.size() > 0){
		cout << "Memory content [" <<"0x" << addrs[0] << ".." <<"0x"<< addrs[1] << "]:" << endl;
		cout << "------------------------------------" << endl;
		for(unsigned int i = addrs[0]; i <= addrs[1]; i = i + 4){
			cout <<"0x"<< i << ": " <<"0x" << address[i] <<endl;
		}
	}
	cout<<endl;
}

vector<unsigned int> splitAddr(string addr){
	vector<unsigned int> result{};
	stringstream ss(addr);
	string temp{};

	while(getline(ss, temp, *colon)) {
		result.push_back(stoi(temp, 0, 16));
	}
	return result;
}

void updateDataAddr(vector<unsigned int> little, map<unsigned int, unsigned int>* addr){
	for(int i = 0; i != little.size(); i++){
		(*addr)[268435456 + i*4] = little[i];
	}
}

vector<unsigned int> bigToLittle(vector<unsigned char> bigE){
	vector<unsigned int> little{};
	int littleIndex = bigE.size()/4;
	for(int i = 0; i < littleIndex; i++){
		unsigned int count = 0;
		for(int j = 0; j < 4; j++){
			if(bigE[(i*4) + j] == 0){
				count += 1;
			}
		}
		unsigned char temp[4] {bigE[i*4 + 3], bigE[i*4 + 2], bigE[i*4 + 1], bigE[i*4]};
		unsigned int value;
		memcpy(&value, temp, sizeof(int));
		value = value >> (count*8);
		little.push_back(value);
	}
	return little;
}

vector<unsigned int> ctrShift(unsigned int fullInst){
	vector<unsigned int> tmp{};
	unsigned int rt = 0;
	unsigned int rd = 0;
	unsigned int shamt = 0;
	for(int i = 6; i < 11; i++){
		shamt += (((fullInst >> i) & 0x01) << (i-6));
	}
	for(int i = 11; i < 16; i++){
		rd += (((fullInst >> i) & 0x01) << (i-11));
	}
	for(int i = 16; i < 21; i++){
		rt += (((fullInst >> i) & 0x01) << (i-16));
	}
	tmp.push_back(rt);
	tmp.push_back(rd);
	tmp.push_back(shamt);

	return tmp;
}

vector<unsigned int> ctrIformat(unsigned int fullInst){
	vector<unsigned int> tmp{};
	unsigned int rs = 0;
	unsigned int rt = 0;
	unsigned int value = 0;
	for(int i = 21; i < 26; i++){
		rs += (((fullInst >> i) & 0x01) << (i-21));
	}
	for(int i = 0; i < 16; i++){
		value += (((fullInst >> i) & 0x01) << (i));
	}
	for(int i = 16; i < 21; i++){
		rt += (((fullInst >> i) & 0x01) << (i-16));
    }
	tmp.push_back(rs);
	tmp.push_back(rt);
	tmp.push_back(value);

	return tmp;
}

vector<unsigned int> ctrRformat(unsigned int fullInst){
	vector<unsigned int> tmp{};
	unsigned int rs = 0;
	unsigned int rt = 0;
	unsigned int rd = 0;
	for(int i = 21; i < 26; i++){
		rs += (((fullInst >> i) & 0x01) << (i-21));
	}
    for(int i = 16; i < 21; i++){
		rt += (((fullInst >> i) & 0x01) << (i-16));
    }
	for(int i = 11; i < 16; i++){
		rd += (((fullInst >> i) & 0x01) << (i-11));
	}
	tmp.push_back(rs);
	tmp.push_back(rt);
	tmp.push_back(rd);
	
	return tmp;
}
int main(int argc, char* argv[])
{
	char m[3] = "-m";
	char d[3] = "-d";
	char n[3] = "-n";

	string searchAddr{};
	bool afterProcess = false;
	int performNum = -1;


	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], m) == 0){
			searchAddr = argv[i+1];
		}
		else if(strcmp(argv[i], d) == 0){
			afterProcess = true;
		}
		else if(strcmp(argv[i], n) == 0){
			performNum = atoi(argv[i+1]);
		}
	}
	
	vector<unsigned int> mAddrs = splitAddr(searchAddr);
	
	vector<unsigned int> lines{};
	unsigned int line = 0;
	
	string fileName(argv[argc-1]);
	ifstream ifas(fileName);

	if(!ifas){
		cerr << "could not open this file" << "\n";
	}
	if(ifas.is_open()){	
		while(ifas >> hex >> line){
			lines.push_back(line);
		}
	}

	ifas.close();

	map<unsigned int, unsigned int> address{};
	map<unsigned int, unsigned int> regs{};

	unsigned int textByte = lines[0];
	unsigned int dataByte = lines[1];
	
	unsigned int textCount = textByte / 4;
	unsigned int dataCount = dataByte / 4;

	for(int i = 0; i < textCount; i++){
		address.insert(pair<unsigned int, unsigned int>(textAddr, lines[i+2]));
		textAddr = textAddr + 4;
	}

	vector<unsigned int> originalData{};

	for(int i = 0; i < dataCount; i++){
		originalData.push_back(lines[i+2+textCount]);
	}

	vector<unsigned char> bigEndianData{};

	for(int i = 0; i != originalData.size(); i++){
		unsigned char* pointer = (unsigned char*)&originalData[i];
		unsigned int count = 0;
		for(int j = 0; j < 4; j++){
			if(pointer[j] == 0){
				count += 1;
			}
		}
		if(count == 0){
			for(int j = 0; j < 4; j++){
				bigEndianData.push_back(pointer[3-j]);
			}
		}
		if(count == 1){
			bigEndianData.push_back(pointer[2]);
			bigEndianData.push_back(pointer[1]);
			bigEndianData.push_back(pointer[0]);
			for(int k = 0; k < count; k++){
				bigEndianData.push_back(0);
			}
		}
		if(count == 2){
			bigEndianData.push_back(pointer[1]);
			bigEndianData.push_back(pointer[0]);
			for(int k = 0; k < count; k++){
				bigEndianData.push_back(0);
			}
		}
		if(count == 3){
			bigEndianData.push_back(pointer[0]);
			for(int k = 0; k < count; k++){
				bigEndianData.push_back(0);
			}
		}
		if(count == 4){
			for(int k = 0; k < count; k++){
				bigEndianData.push_back(0);
			}
		}
	}

	for(int i = 0; i < dataCount; i++){
		address.insert(pair<unsigned int, unsigned int>(dataAddr, lines[i+2+textCount]));
		dataAddr += 4;
	}
	
	for(int i = 0; i < 32; i++){
		regs.insert(pair<unsigned int, unsigned int>(i, 0));
	}
	
	bool isLimit = true;
	
	if(performNum == -1){
		isLimit = false;
	}
	if(performNum == 0){
		printOutput(currPc, regs, mAddrs, address);
		return 0;
	}
	unsigned int endAddr = 4194304 + (textCount*4);
	unsigned int count = 0;
	bool isExit = false;	

	while(currPc < endAddr){
		bool isChange = false;
		unsigned int moveAddr = 0;
		unsigned int fullInst = address[currPc];
		unsigned int checkOp = 0;
		for(int j = 26; j < 32; j++){
			checkOp += (((fullInst >> j) & 0x01) << (j-26));
		}
		if(checkOp == 0){ // R format
			unsigned int checkFunct = 0;
			for(int k = 0; k < 6; k++){
				checkFunct += (((fullInst >> k) & 0x01) << k);
			}
			if(checkFunct == 33){ // addu
				auto adduArr = ctrRformat(fullInst);
				if(regs[adduArr[0]] + regs[adduArr[1]] >= 0x100000000){
					regs[adduArr[2]] = regs[adduArr[0]] + regs[adduArr[1]] - 0x100000000;
				}
				regs[adduArr[2]] = regs[adduArr[0]] + regs[adduArr[1]]; // rd = rs + rt
			}
			if(checkFunct == 36){ // and
				auto andArr = ctrRformat(fullInst);
				regs[andArr[2]] = regs[andArr[0]] & regs[andArr[1]]; // rd = rs & rt
			}
			if(checkFunct == 8){  // jr
				int rs = 0;
				for(int h = 21; h < 26; h++){
					rs += (((fullInst >> h) & 0x01) << (h-21));
				}
				moveAddr = currPc;
				currPc = regs[rs];
				isChange = true;
			}
			if(checkFunct == 39){  // nor
				auto norArr = ctrRformat(fullInst);
				regs[norArr[2]] = ~(regs[norArr[0]] | regs[norArr[1]]);
			}
			if(checkFunct == 37){  // or
				auto orArr = ctrRformat(fullInst);
				regs[orArr[2]] = (regs[orArr[0]] | regs[orArr[1]]);
			}
			if(checkFunct == 43){  // sltu
				auto sltuArr = ctrRformat(fullInst);
				if(regs[sltuArr[0]] < regs[sltuArr[1]]){
					regs[sltuArr[2]] = 1;
				}
				else {
					regs[sltuArr[2]] = 0;
				}
			}
			if(checkFunct == 0){  // sll
				auto sllArr = ctrShift(fullInst);
				regs[sllArr[1]] = (regs[sllArr[0]] << sllArr[2]);
			}
			if(checkFunct == 2){  // srl
				auto srlArr = ctrShift(fullInst);
				regs[srlArr[1]] = (regs[srlArr[0]] >> srlArr[2]);
			}
			if(checkFunct == 35){  // subu
				auto subuArr = ctrRformat(fullInst);
				regs[subuArr[2]] = regs[subuArr[0]] - regs[subuArr[1]];
			}
		}
		else { // I format or J format
			if(checkOp == 9){  // addiu
				auto addiuArr = ctrIformat(fullInst);
				signed short temp = addiuArr[2];
				if((regs[addiuArr[0]] + temp) >= 0x100000000){
					regs[addiuArr[1]] = regs[addiuArr[0]] + temp - 0x100000000;
				}
				else {
					regs[addiuArr[1]] = regs[addiuArr[0]] + temp; // rt = rs + imm
				}
				checkOp = 0;
			}
			if(checkOp == 12){  // andi
				auto andiArr = ctrIformat(fullInst);
				regs[andiArr[1]] = regs[andiArr[0]] & andiArr[2]; // rt = rs & imm
				checkOp = 0;
			}
			if(checkOp == 4){  // beq
				auto beqArr = ctrIformat(fullInst);
				if(regs[beqArr[0]] == regs[beqArr[1]]){
					moveAddr = currPc;
					currPc = currPc + 4 + beqArr[2]*4; // branch to target address
					isChange = true;
				}
				checkOp = 0;
			}
			if(checkOp == 5){  // bne
				auto bneArr = ctrIformat(fullInst);
				if(regs[bneArr[0]] != regs[bneArr[1]]){
					moveAddr = currPc;
					currPc = currPc + 4 + bneArr[2]*4; // branch to target address
					isChange = true;
				}
				checkOp = 0;
			}
			if(checkOp == 2){  // j
				unsigned int fourBitPc = 0;
				for(int g = 28; g < 32; g++){
					fourBitPc += (((currPc >> g) & 0x01) << g);
				}
				unsigned int target = 0;
				for(int f = 0; f < 26; f++){
					target += (((fullInst >> f) & 0x01) << f);
				}
				moveAddr = currPc;
				currPc = fourBitPc + (4*target);
				if(currPc == endAddr){
					isExit = true;
				}
				isChange = true;
				checkOp = 0;
			}
			if(checkOp == 3){  // jal
				regs[31] = currPc + 4;
				unsigned int fourBitPc = 0;
				for(int d = 28; d < 32; d++){
					fourBitPc += (((currPc >> d) & 0x01) << (d-28));
				}
				unsigned int target = 0;
				for(int s = 0; s < 26; s++){
					target += (((fullInst >> s) & 0x01) << s);
				}
				moveAddr = currPc;
				currPc = fourBitPc + (4*target);
				isChange = true;
				checkOp = 0;
			}
			if(checkOp == 15){  // lui
				auto luiArr = ctrIformat(fullInst);
				regs[luiArr[1]] = (luiArr[2] << 16);
				checkOp = 0;
			}
			if(checkOp == 35){  // lw
				auto lwArr = ctrIformat(fullInst);
				int findIndex = 0;
				for(int q = 0; q != originalData.size(); q++){
					if(originalData[q] == address[regs[lwArr[0]]]){
						findIndex = q;
					}
				}
				vector<unsigned char> lwChar;
				for(int w = 0; w < 4; w++){
					lwChar.push_back(bigEndianData[findIndex*4 + w + lwArr[2]]);
				}
				unsigned int littleData = 0;
				littleData = ((unsigned int)lwChar[0] << 0) |
							 ((unsigned int)lwChar[1] << 8) |
							 ((unsigned int)lwChar[2] << 16) |
							 ((unsigned int)lwChar[3] << 24);
				regs[lwArr[1]] = littleData;
				checkOp = 0;
			}
			if(checkOp == 32){  // lb
				auto lbArr = ctrIformat(fullInst);
				int findIndex = 0;
				for(int j = 0; j != originalData.size(); j++){
					if(originalData[j] == address[regs[lbArr[0]]]){
						findIndex = j;
					}
				}
				char lbChar = 0;
				lbChar = bigEndianData[findIndex*4 + lbArr[2]];
				int littleData = 0;
				littleData = (int)lbChar;
				regs[lbArr[1]] = littleData;
				checkOp = 0;
			}
			if(checkOp == 13){  // ori
				auto oriArr = ctrIformat(fullInst);
				regs[oriArr[1]] = (regs[oriArr[0]] | oriArr[2]);
				checkOp = 0;
			}
			if(checkOp == 11){  // sltiu
				auto sltiuArr = ctrIformat(fullInst);
				signed short temp = sltiuArr[2];
				if(regs[sltiuArr[0]] < temp){
					regs[sltiuArr[1]] = 1;
				}
				else {
					regs[sltiuArr[1]] = 0;
				}
				checkOp = 0;
			}
			if(checkOp == 43){  // sw
				auto swArr = ctrIformat(fullInst);
				int findIndex;
				for(int j = 0; j != originalData.size(); j++){
					if(originalData[j] == address[regs[swArr[0]]]){
						findIndex = j;
					}
				}
				unsigned char* pointer = (unsigned char*)&regs[swArr[1]];
				unsigned int zeroCount = 0;
				for(int j = 0; j < 4; j++){
					if(pointer[j] == 0){
						zeroCount += 1;
					}
				}
				if(zeroCount == 0){
					for(int j = 0; j < 4; j++){
						bigEndianData[findIndex*4 + swArr[2] + j] = pointer[3-j];
					}
				}
				if(zeroCount == 1){
					bigEndianData[findIndex*4 + swArr[2]] = pointer[2];
					bigEndianData[findIndex*4 + swArr[2] + 1] = pointer[1];
					bigEndianData[findIndex*4 + swArr[2] + 2] = pointer[0];
					bigEndianData[findIndex*4 + swArr[2] + 3] = 0;
				}
				if(zeroCount == 2){
					bigEndianData[findIndex*4 + swArr[2]] = pointer[1];
					bigEndianData[findIndex*4 + swArr[2] + 1] = pointer[0];
					bigEndianData[findIndex*4 + swArr[2] + 2] = 0;
					bigEndianData[findIndex*4 + swArr[2] + 3] = 0;
				}
				if(zeroCount == 3){
					bigEndianData[findIndex*4 + swArr[2]] = pointer[0];
					bigEndianData[findIndex*4 + swArr[2] + 1] = 0;
					bigEndianData[findIndex*4 + swArr[2] + 2] = 0;
					bigEndianData[findIndex*4 + swArr[2] + 3] = 0;
				}
				if(zeroCount == 4){
					for(int j = 0; j < 4; j++){
						bigEndianData[findIndex*4 + swArr[2] + j] = 0;
					}
				}
				vector<unsigned int> little = bigToLittle(bigEndianData);
				updateDataAddr(little, &address);
				checkOp = 0;
			}
			if(checkOp == 40){  // sb
				auto sbArr = ctrIformat(fullInst);
				int findIndex;
				for(int j = 0; j != originalData.size(); j++){
					if(originalData[j] == address[regs[sbArr[0]]]){
						findIndex = j;
					}
				}
				unsigned char temp = 0;
				temp = regs[sbArr[1]];
				bigEndianData[findIndex*4 + sbArr[2]] = temp;
				vector<unsigned int> little = bigToLittle(bigEndianData);
				updateDataAddr(little, &address);
				checkOp = 0;
			}
		}
		if(isLimit == true){
			count += 1;
		}
		if(isChange == true){
			printOutput(moveAddr, regs, mAddrs, address);
		}
		else {
			if(afterProcess == true){
				printOutput(currPc, regs, mAddrs, address);
			}
			currPc += 4;
		}
		if(isExit == true){
			printOutput(currPc, regs, mAddrs, address);
		}
		if(count == performNum){
			break;
		}
	}
	if(afterProcess == false){
 		printOutput(currPc, regs, mAddrs, address);
	}

	return 0;	
}
