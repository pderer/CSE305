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

int checkOp(unsigned int inst){
	int op = 0;
	for(int j = 26; j < 32; j++){
		op += (((inst >> j) & 0x01) << (j-26));
	}
	return op;
}

int checkFunct(unsigned int inst){
	unsigned int funct = 0;
	for(int k = 0; k < 6; k++){
		funct += (((inst >> k) & 0x01) << k);
	}
	return funct;
}

class Control {
	private:
	bool RW = false;
	bool MR = false;
	bool MW = false;
	bool isSame = false;

	public:
	void setRW(bool tf){ RW = tf; }
	bool getRW(){ return RW; }
	void setMR(bool tf){ MR = tf; }
	bool getMR(){ return MR; }
	void setMW(bool tf){ MW = tf; }
	bool getMW(){ return MW; }
	void setSame(bool tf){ isSame = tf; }
	bool getSame(){ return isSame; }

};

class IFID {
	private:
	bool isRun = false;
	bool isExit = false;
	unsigned int inst;
	unsigned int NPC;
	unsigned int rs, rt;

	public:
	void setInst(unsigned int instruction);
	unsigned int getInst(){ return inst; }
	void setRun(bool tf);
	bool getRun(){ return isRun; }
	void setExit(bool tf){ isExit = tf; }
	bool getExit(){ return isExit; }
	void setNPC(unsigned int pc);
	unsigned int getNPC(){ return NPC; }
	void setRs(unsigned int s){ rs = s; }
	unsigned int getRs(){ return rs; }
	void setRt(unsigned int t){ rt = t; }
	unsigned int getRt(){ return rt; }
	void update(class IFID);
};

void IFID::setInst(unsigned int instruction){ inst = instruction; }
void IFID::setRun(bool tf){ isRun = tf; }
void IFID::setNPC(unsigned int pc){ NPC = pc + 4; }
void IFID::update(IFID endIF){
	inst = endIF.getInst();
	NPC = endIF.getNPC();
	isRun = endIF.getRun();
	isExit = endIF.getExit();
	rs = endIF.getRs();
	rt = endIF.getRt();
}

class IDEX : public Control {
	private:
	bool isRun = false;
	bool isExit = false;
	unsigned int NPC, IMM;
	unsigned int rs, rt, rd, shamt;
	unsigned int rsData, rtData;
	string inst, type;

	public:
	void setRun(bool tf){ isRun = tf; }
	bool getRun(){ return isRun; }
	void setExit(bool tf){ isExit = tf; }
	bool getExit(){ return isExit; }
	void setNPC(unsigned int num){ NPC = num; }
	unsigned int getNPC(){ return NPC; }
	void setIMM(unsigned int im){ IMM = im; }
	unsigned int getIMM(){ return IMM; }
	void setRs(unsigned int s){ rs = s; }
	unsigned int getRs(){ return rs; }
	void setRt(unsigned int t){ rt = t; }
	unsigned int getRt(){ return rt; }
	void setRd(unsigned int d){ rd = d; }
	unsigned int getRd(){ return rd; }
	void setInst(string ins){ inst = ins; }
	string getInst(){ return inst; }
	void setType(string ty){ type = ty; }
	string getType(){ return type; }
	void setShamt(unsigned int sh){ shamt = sh; }
	unsigned int getShamt(){ return shamt; }
	void setRsData(unsigned int s){ rsData = s; }
	unsigned int getRsData(){ return rsData; }
	void setRtData(unsigned int t){ rtData = t; }
	unsigned int getRtData(){ return rtData; }
	void update(class IDEX);
};

void IDEX::update(IDEX endID){
	isRun = endID.getRun();
	isExit = endID.getExit();
	NPC = endID.getNPC();
	IMM = endID.getIMM();
	rs = endID.getRs();
	rt = endID.getRt();
	rd = endID.getRd();
	shamt = endID.getShamt();
	inst = endID.getInst();
	type = endID.getType();
	rsData = endID.getRsData();
	rtData = endID.getRtData();
	//update control
	setRW(endID.getRW());
	setMR(endID.getMR());
	setMW(endID.getMW());
	setSame(endID.getSame());
}

class EXMEM : public Control {
	private:
	bool isRun = false;
	bool isExit = false;
	unsigned int NPC, rs, rt, rd, target, ALU;
	string inst, type;

	public:
	void setRun(bool tf){ isRun = tf; }
	bool getRun(){ return isRun; }
	void setExit(bool tf){ isExit = tf; }
	bool getExit(){ return isExit; }
	void setNPC(unsigned int num){ NPC = num; }
	unsigned int getNPC(){ return NPC; }
	void setRs(unsigned int s){ rs = s; }
	unsigned int getRs(){ return rs; }
	void setRt(unsigned int t){ rt = t; }
	unsigned int getRt(){ return rt; }
	void setRd(unsigned int d){ rd = d; }
	unsigned int getRd(){ return rd; }
	void setTarget(unsigned int tg){ target = tg; }
	unsigned int getTarget(){ return target; }
	void setALU(unsigned int a){ ALU = a; }
	unsigned int getALU(){ return ALU; }
	void setInst(string ins){ inst = ins; }
	string getInst(){ return inst; }
	void setType(string ty){ type = ty; }
	string getType(){ return type; }
	void update(class EXMEM);
};

void EXMEM::update(EXMEM endEX){
	isRun = endEX.getRun();
	isExit = endEX.getExit();
	NPC = endEX.getNPC();
	rs = endEX.getRs();
	rt = endEX.getRt();
	rd = endEX.getRd();
	target = endEX.getTarget();
	ALU = endEX.getALU();
	type = endEX.getType();
	inst = endEX.getInst();
	//update control
	setRW(endEX.getRW());
	setMR(endEX.getMR());
	setMW(endEX.getMW());
	setSame(endEX.getSame());
}

class MEMWB : public Control {
	private:
	bool isRun = false;
	bool isExit = false;
	unsigned int NPC, rs, rt, rd, target, ALU;
	string inst, type;
	
	public:
	void setRun(bool tf){ isRun = tf; }
	bool getRun(){ return isRun; }
	void setExit(bool tf){ isExit = tf; }
	bool getExit(){ return isExit; }
	void setNPC(unsigned int num){ NPC = num; }
	unsigned int getNPC(){ return NPC; }
	void setRs(unsigned int s){ rs = s; }
	unsigned int getRs(){ return rs; }
	void setRt(unsigned int t){ rt = t; }
	unsigned int getRt(){ return rt; }
	void setRd(unsigned int d){ rd = d; }
	unsigned int getRd(){ return rd; }
	void setTarget(unsigned int tg){ target = tg; }
	unsigned int getTarget(){ return target; }
	void setALU(unsigned int a){ ALU = a; }
	unsigned int getALU(){ return ALU; }
	void setInst(string ins){ inst = ins; }
	string getInst(){ return inst; }
	void setType(string ty){ type = ty; }
	string getType(){ return type; }
	void update(class MEMWB);
};

void MEMWB::update(MEMWB endMEM){
	isRun = endMEM.getRun();
	isExit = endMEM.getExit();
	NPC = endMEM.getNPC();
	rs = endMEM.getRs();
	rt = endMEM.getRt();
	rd = endMEM.getRd();
	target = endMEM.getTarget();
	ALU = endMEM.getALU();
	type = endMEM.getType();
	inst = endMEM.getInst();
	//update control
	setRW(endMEM.getRW());
	setMR(endMEM.getMR());
	setMW(endMEM.getMW());
	setSame(endMEM.getSame());
}

void printOutput(unsigned int currPc, map<unsigned int, unsigned int> regs,vector<unsigned int> addrs, map<unsigned int, unsigned int> address, int cycle, string pipeArray[5], bool completion, bool pOption, bool dOption){
	if(completion){
		cout << "===== Completion cycle: "<< dec << cycle <<" ====="<<endl;
	} else {
		cout << "===== Cycle "<< dec << cycle << " ====="<<endl;
	}
	if(pOption || completion){
		cout << "Current pipeline PC state:"<<endl;
		cout << "{" << pipeArray[0] << "|" << pipeArray[1] << "|" << pipeArray[2] << "|" << pipeArray[3] << "|" << pipeArray[4] << "}" << endl << endl;
	}
	if(dOption || completion){
		cout << "Current register values:" <<endl;
		cout << "------------------------------------" << endl;
		cout << "PC: " <<"0x"<< hex << currPc << endl;
		cout << "Registers:" << endl;
		for(int i = 0; i < 32; i++){
			cout << "R"<< dec << i << ": "<<"0x"<< hex << regs[i] << endl;
		}
	}
	if(addrs.size() > 0){
		cout << endl;
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
		unsigned char temp[4] {bigE[i*4 + 3], bigE[i*4 + 2], bigE[i*4 + 1], bigE[i*4]};
		unsigned int value;
		memcpy(&value, temp, sizeof(int));
		if((value & 0x0000FF00) == 0 && (value & 0x00FF0000) != 0 && (value & 0xFF000000) == 0 && (value & 0x000000FF) == 0) {
			value = value >> 8;
		} else {
			if((value & 0x000000FF) == 0){
				value = value >> 8;
			}
			if((value & 0x000000FF) == 0){
				value = value >> 8;
			}
			if((value & 0x000000FF) == 0){
				value = value >> 8;
			}
			if((value & 0x000000FF) == 0){
				value = value >> 8;
			}
		}
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

unsigned int ctrJformat(unsigned int NPC, unsigned int inst){
	unsigned int fourBitPc = 0;
	for(int g = 28; g < 32; g++){
		fourBitPc += (((NPC >> g) & 0x01) << g);
	}
	unsigned int target = 0;
	for(int f = 0; f < 26; f++){
		target += (((inst >> f) & 0x01) << f);		
	}
	return (fourBitPc + (4*target));
}

string intToHex(int num){
	stringstream ss;
	ss << "0x" << hex << num;
	return ss.str();
}

int main(int argc, char* argv[])
{
	char m[3] = "-m";
	char d[3] = "-d";
	char n[3] = "-n";
	char p[3] = "-p";

	string searchAddr{};
	bool afterProcess = false;
	int performNum = -1;
	bool showPipe = false;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], m) == 0){
			searchAddr = argv[i+1];
		}
		else if(strcmp(argv[i], d) == 0){
			afterProcess = true;
		}
		else if(strcmp(argv[i], p) == 0){
			showPipe = true;
		}
		else if(strcmp(argv[i], n) == 0){
			performNum = atoi(argv[i+1]);
		}
	}
	
	// branch predictor
	char atp[5] = "-atp";
	char antp[6]= "-antp";
	bool isAtp = false;
	if(strcmp(argv[1], atp) == 0 || strcmp(argv[1], antp) == 0){
		if(strcmp(argv[1], atp) == 0){
			isAtp = true;
		}		
	} else {
		cerr << "there is no branch predictor option" << endl;
		return 0;
	}
	

	vector<unsigned int> mAddrs = splitAddr(searchAddr);
	
	vector<unsigned int> lines{};
	unsigned int line = 0;
	
	string fileName(argv[argc-1]);
	ifstream ifas(fileName);

	if(!ifas){
		cerr << "could not open this file" << "\n";
		return 0;
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
		if(originalData[i] == 0){
			for(int k = 0; k < 4; k++){
				bigEndianData.push_back(0);
			}
		}
		if(originalData[i] <= 0xFF && originalData[i] != 0){
			for(int k = 0; k < 3; k++){
				bigEndianData.push_back(0);
			}
			bigEndianData.push_back(pointer[0]);
		}
		if(originalData[i] <= 0xFFFF && originalData[i] > 0xFF){
			if(pointer[0] == 0){
				bigEndianData.push_back(pointer[0]);
				bigEndianData.push_back(pointer[1]);
			} else {
				bigEndianData.push_back(pointer[1]);
				bigEndianData.push_back(pointer[0]);
			}
			for(int k = 0; k < 2; k++){
				bigEndianData.push_back(0);
			}
		}
		if(originalData[i] <= 0xFFFFFF && originalData[i] > 0xFFFF){
			bigEndianData.push_back(pointer[2]);
			bigEndianData.push_back(pointer[1]);
			bigEndianData.push_back(pointer[0]);
			for(int k = 0; k < 1; k++){
				bigEndianData.push_back(0);
			}	
		}
		if(originalData[i] <= 0xFFFFFFFF && originalData[i] > 0xFFFFFF){
			for(int j = 0; j < 4; j++){
				bigEndianData.push_back(pointer[3-j]);
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
	string pipeOutPutArray[5]={};
	if(performNum == -1){
		isLimit = false;
	}
	if(performNum == 0){
		printOutput(currPc, regs, mAddrs, address, 0, pipeOutPutArray, true, showPipe, afterProcess);
		return 0;
	}
	unsigned int endAddr = 4194304 + (textCount*4);
	unsigned int instCount = 0;
	bool isExitLoop = false;	
	bool haveExit = false;
	bool whileExit = false;

	class IFID IFIDReg;
	class IDEX IDEXReg;
	class EXMEM EXMEMReg;
	class MEMWB MEMWBReg;
	
	int totalCycle = 0;
	
	bool pcWrite = true;
	
	while(true){
		// IF Stage (must doing process)
		class IFID endIF;
		class IDEX endID;
		class EXMEM endEX;
		class MEMWB endMEM;
		
		bool pcChange = true;

		endIF.setRun(true);
		if(pcWrite == false){
			currPc -= 4;
			pcWrite = true;
		}
		if(currPc < endAddr){
			unsigned int ins = address[currPc];
			endIF.setInst(ins);
			endIF.setNPC(currPc); // set IFIDReg NPC - 4, IDEX flush
			auto loadArr = ctrIformat(ins); // for load use hazard
			endIF.setRs(loadArr[0]);
			endIF.setRt(loadArr[1]);
			pipeOutPutArray[0] =  intToHex(endIF.getNPC() - 4);
			if(haveExit){
				if(isExitLoop){
					endIF.setExit(true);
				}
			}
			if(haveExit == false){
				if(currPc == endAddr - 4){
					endIF.setExit(true);
				}
			}
		} else if(currPc == endAddr && haveExit){ // have Exit and show exit address on pipeline
			endIF.setRun(false);
			pipeOutPutArray[0] = "";
			endIF.setNPC(currPc - 4);
		}else {
			endIF.setRun(false);
			pipeOutPutArray[0] = "";
		}
		// WB stage
		// for structural hazard
		if(MEMWBReg.getRun()){
			pipeOutPutArray[4] = intToHex(MEMWBReg.getNPC() - 4);
			if(MEMWBReg.getType() == "r" || MEMWBReg.getType() == "shift"){
				cout<<"MEMWB's inst :"<<MEMWBReg.getInst()<<endl;
				regs[MEMWBReg.getRd()] = MEMWBReg.getALU();
			}
			if(MEMWBReg.getType() == "i"){
				if(MEMWBReg.getInst() != "beq" && MEMWBReg.getInst() != "bne"){
					regs[MEMWBReg.getRt()] = MEMWBReg.getALU();
				}
			}
			if(MEMWBReg.getInst() == "jal"){
				regs[31] = MEMWBReg.getNPC();
			}
			if(haveExit){
				if(MEMWBReg.getExit()){
					whileExit = true;
				}
			}
			if(haveExit == false){
				if(MEMWBReg.getExit()){
					whileExit = true;
				}
			}
			if(isLimit){
				instCount += 1;
				if(instCount == performNum){		
					whileExit = true;
				}
			}
		} else {
			pipeOutPutArray[4] = "";
		}
		// ID stage
		if(IFIDReg.getRun()){ // else pipeOutPutArray[1] = "" bc of flush
			unsigned int inst = IFIDReg.getInst();
			endID.setNPC(IFIDReg.getNPC());
			endID.setExit(IFIDReg.getExit());
			pipeOutPutArray[1] = intToHex(endID.getNPC() - 4);
			endID.setRun(true);
		   	if(checkOp(inst) == 0){ // R format
		   		if(checkFunct(inst) == 33 || checkFunct(inst) == 36 || checkFunct(inst) == 39 || checkFunct(inst) == 37 || checkFunct(inst) == 35 || checkFunct(inst) == 43){ // addu, and, nor, or, subu, sltu
					auto stdArr = ctrRformat(inst);
					endID.setRs(stdArr[0]);
					endID.setRt(stdArr[1]);
					endID.setRd(stdArr[2]);
					endID.setRsData(regs[stdArr[0]]);
					endID.setRtData(regs[stdArr[1]]);
					endID.setRW(true);
					endID.setType("r");
					if(checkFunct(inst) == 33){ // addu
						endID.setInst("addu");
					}
					if(checkFunct(inst) == 36){ // and
						endID.setInst("and");
					}
					if(checkFunct(inst) == 39){ // nor
						endID.setInst("nor");
					}
					if(checkFunct(inst) == 37){ // or
						endID.setInst("or");
					}
					if(checkFunct(inst) == 35){ // subu
						endID.setInst("subu");
					}
					if(checkFunct(inst) == 43){ // sltu
						endID.setInst("sltu");
					}
				}

				if(checkFunct(inst) == 8){ // jr // need to flush 1 cycle
					auto jrArr = ctrRformat(inst);
					endIF.setRun(false); // flush
					currPc = regs[jrArr[0]]; // jump
					pcChange = false;
					haveExit = true;
				}
				if(checkFunct(inst) == 0 || checkFunct(inst) == 2){ // sll, srl
					auto shArr = ctrShift(inst);
					endID.setRt(shArr[0]);
					endID.setRtData(regs[shArr[0]]);
					endID.setRd(shArr[1]);
					endID.setShamt(shArr[2]);
					endID.setRW(true);
					endID.setType("shift");
					if(checkFunct(inst) == 0){
						endID.setInst("sll");
					}
					if(checkFunct(inst) == 2){
						endID.setInst("srl");
					}
				}
		   	}
			else {  // I or J format
				if(checkOp(inst) == 9 || checkOp(inst) == 12 || checkOp(inst) == 4 || checkOp(inst) == 5 || checkOp(inst) == 15 || checkOp(inst) == 35 || checkOp(inst) == 32 || checkOp(inst) == 13 || checkOp(inst) == 11 || checkOp(inst) == 43 || checkOp(inst) == 40){
					auto IArr = ctrIformat(inst);
					endID.setRs(IArr[0]);
					endID.setRt(IArr[1]);
					endID.setRsData(regs[IArr[0]]);
					endID.setRtData(regs[IArr[1]]);
					endID.setIMM(IArr[2]);
					endID.setRW(true);
					endID.setType("i");
					if(checkOp(inst) == 9){ // addiu
						endID.setInst("addiu");
					}
					if(checkOp(inst) == 12){ // andi
						endID.setInst("andi");
					}
					if(isAtp){
						if(checkOp(inst) == 4){ // beq
							endID.setInst("beq");
							currPc = 4*endID.getIMM() + endID.getNPC();
							pcChange = false;
							haveExit = true;
							endIF.setRun(false);
						}
						if(checkOp(inst) == 5){ // bne
							endID.setInst("bne");
							currPc = 4*endID.getIMM() + endID.getNPC();
							pcChange = false;
							haveExit = true;
							endIF.setRun(false);
						}
					}
					if(isAtp == false){
						if(checkOp(inst) == 4){
							endID.setInst("beq");
							haveExit = true;
						}
						if(checkOp(inst) == 5){
							endID.setInst("bne");
							haveExit = true;
						}
					}
					if(checkOp(inst) == 15){ // lui
						endID.setInst("lui");
					}
					if(checkOp(inst) == 35){ // lw
						endID.setInst("lw");
						endID.setMR(true);
					}
					if(checkOp(inst) == 32){ // lb
						endID.setInst("lb");
						endID.setMR(true);
					}
					if(checkOp(inst) == 13){ // ori
						endID.setInst("ori");
					}
					if(checkOp(inst) == 11){ // sltiu
						endID.setInst("sltiu");
					}
					if(checkOp(inst) == 43){ // sw
						endID.setInst("sw");
						endID.setMW(true);
						endID.setRW(false);
					}
					if(checkOp(inst) == 40){ // sb
						endID.setInst("sb");
						endID.setMW(true);
						endID.setRW(false);
					}
				}
				if(checkOp(inst) == 2 || checkOp(inst) == 3){ // j, jal
					auto target = ctrJformat(IFIDReg.getNPC(), inst);
					endIF.setRun(false); // flush
					endID.setType("j");
					currPc = target;
					haveExit = true;
					if(currPc == endAddr){
						endID.setExit(true);
						isExitLoop = true;
					}
					pcChange = false;
					if(checkOp(inst) == 3){ // jal
						endID.setRW(true);
						endID.setInst("jal");
					}
				}
			} 
			// end ID stage, need to update IDEX, output array update
		} else {
			pipeOutPutArray[1] = "";	
		}
		// EX Stage
		if(IDEXReg.getRun()){
			endEX.setRun(true);
			endEX.setNPC(IDEXReg.getNPC());
			endEX.setRs(IDEXReg.getRs());
			endEX.setRt(IDEXReg.getRt());
			endEX.setRd(IDEXReg.getRd());
			endEX.setType(IDEXReg.getType());
			endEX.setInst(IDEXReg.getInst());
			endEX.setRW(IDEXReg.getRW());
			endEX.setMW(IDEXReg.getMW());
			endEX.setMR(IDEXReg.getMR());
			endEX.setSame(IDEXReg.getSame());
			endEX.setExit(IDEXReg.getExit());
			pipeOutPutArray[2] = intToHex(endEX.getNPC() - 4);
			if(IDEXReg.getType() == "r"){
				unsigned int rsData = IDEXReg.getRsData();
				unsigned int rtData = IDEXReg.getRtData();
				int rd = IDEXReg.getRd();
				if(IDEXReg.getInst() == "addu"){
					if(rsData + rtData >= 0x100000000){
						endEX.setALU(rsData + rtData - 0x100000000);
					}
					endEX.setALU(rsData + rtData);
				}
				if(IDEXReg.getInst() == "and"){
					endEX.setALU(rsData & rtData);
				}
				if(IDEXReg.getInst() == "nor"){
					endEX.setALU(~(rsData | rtData));
				}
				if(IDEXReg.getInst() == "or"){
					endEX.setALU(rsData | rtData);
				}
				if(IDEXReg.getInst() == "sltu"){
					if(rsData < rtData){
						endEX.setALU(1);
					} else {
						endEX.setALU(0);
					}
				}
				if(IDEXReg.getInst() == "subu"){
					endEX.setALU(rsData - rtData);
				}
			}
			if(IDEXReg.getType() == "shift"){
				unsigned int rtData = IDEXReg.getRtData();
				int rd = IDEXReg.getRd();
				int shamt = IDEXReg.getShamt();
				if(IDEXReg.getInst() == "sll"){
					endEX.setALU(rtData << shamt);
				}
				if(IDEXReg.getInst() == "srl"){
					endEX.setALU(rtData >> shamt);
				}
			}
			if(IDEXReg.getType() == "i"){
				unsigned int rsData = IDEXReg.getRsData();
				unsigned int rtData = IDEXReg.getRtData();
				unsigned int imm = IDEXReg.getIMM();
				endEX.setRd(IDEXReg.getRt());
				if(IDEXReg.getInst() == "addiu"){
					signed short temp = imm;
					if((rsData + temp) >= 0x100000000){
						endEX.setALU(rsData + temp - 0x100000000);
					} else {
						endEX.setALU(rsData + temp);
					}
				}
				if(IDEXReg.getInst() == "andi"){
					endEX.setALU(rsData & imm);
				}
				if(IDEXReg.getInst() == "beq"){
					if(rsData == rtData){
						endEX.setSame(true);
						endEX.setTarget(4 * imm);
					} else {
						endEX.setSame(false);
					}
					// atp : beq but its not same (isSame = false), flush 3 cycle
					// antp : beq but its same (isSame = true), flush 3 cycle
				}
				if(IDEXReg.getInst() == "bne"){
					if(rsData != rtData){
						endEX.setSame(false);
						endEX.setTarget(4 * imm);
					} else {
						endEX.setSame(true);
					}
					// atp : bne but its same (isSame = true), flush 3 cycle
					// antp : bne but its not same (isSame = false), flush 3 cycle
				}
				if(IDEXReg.getInst() == "lui"){
					endEX.setALU(imm << 16);
				}
				if(IDEXReg.getInst() == "lw" || IDEXReg.getInst() == "lb"){
					int findIndex = (rsData - 0x10000000) / 4;
					endEX.setTarget(findIndex*4 + imm); // findIndex*4 + imm
				}
				if(IDEXReg.getInst() == "ori"){
					endEX.setALU(rsData | imm);
				}
				if(IDEXReg.getInst() == "sltiu"){
					signed short temp = imm;
					if(rsData < temp){
						endEX.setALU(1);
					} else {
						endEX.setALU(0);
					}
				}
				if(IDEXReg.getInst() == "sw"){
					int findIndex = (rsData - 0x10000000) / 4;
					endEX.setTarget(findIndex*4 + imm);
					endEX.setALU(rtData);
				}
				if(IDEXReg.getInst() == "sb"){
					int findIndex = (rsData - 0x10000000) / 4;
					endEX.setTarget(findIndex*4 + imm);
					endEX.setALU((unsigned char)rtData);
				}
			}
		} else {
			pipeOutPutArray[2] = "";
		}
		// MEM Stage
		if(EXMEMReg.getRun()){
			endMEM.setRun(true);
			endMEM.setNPC(EXMEMReg.getNPC());
			endMEM.setRs(EXMEMReg.getRs());
			endMEM.setRt(EXMEMReg.getRt());
			endMEM.setRd(EXMEMReg.getRd());
			endMEM.setType(EXMEMReg.getType());
			endMEM.setInst(EXMEMReg.getInst());
			endMEM.setRW(EXMEMReg.getRW());
			endMEM.setMW(EXMEMReg.getMW());
			endMEM.setMR(EXMEMReg.getMR());
			endMEM.setSame(EXMEMReg.getSame());
			endMEM.setTarget(EXMEMReg.getTarget());
			endMEM.setALU(EXMEMReg.getALU());
			endMEM.setExit(EXMEMReg.getExit());
			pipeOutPutArray[3] = intToHex(EXMEMReg.getNPC() - 4);
			// r type and shift type dont do anything in MEM Stage
			if(EXMEMReg.getType() == "i"){
				endMEM.setRd(EXMEMReg.getRt()); // rd = rt for data hazard, i have to implement EXforward too.
			}
			if(isAtp){ // atp
				if(EXMEMReg.getInst() == "beq"){
					if(EXMEMReg.getSame() == false){ // 3 cycle flush are needed, fetch next inst
						currPc = EXMEMReg.getNPC();
						pcChange = false;
						endIF.setRun(false);
						endID.setRun(false);
					} else {
						if(endAddr == (EXMEMReg.getTarget() + EXMEMReg.getNPC())){
							endMEM.setExit(true);
							isExitLoop = true;
						}
					}
				}
				if(EXMEMReg.getInst() == "bne"){
					if(EXMEMReg.getSame()){
						currPc = EXMEMReg.getNPC(); // 3 cycle flush are needed, fetch next inst
						pcChange = false;
						endIF.setRun(false);
						endID.setRun(false);
					} else {
						if(endAddr == (EXMEMReg.getTarget() + EXMEMReg.getNPC())){
							endMEM.setExit(true);
							isExitLoop = true;
						}
					}
				}
			}
			if(isAtp == false){ // antp
				if(EXMEMReg.getInst() == "beq"){
					if(EXMEMReg.getSame()){ // 3 cycle flush, fetch target address
						currPc = EXMEMReg.getTarget() + EXMEMReg.getNPC();
						if(currPc == endAddr){
							endMEM.setExit(true);
							isExitLoop = true;
						}
						pcChange = false;
						endIF.setRun(false);
						endID.setRun(false);
						endEX.setRun(false);
					}
				}
				if(EXMEMReg.getInst() == "bne"){
					if(EXMEMReg.getSame() == false){ // 3 cycle flush, fetch target address
						currPc = EXMEMReg.getTarget() + EXMEMReg.getNPC();
						if(currPc == endAddr){
							endMEM.setExit(true);
							isExitLoop = true;
						}
						pcChange = false;
						endIF.setRun(false);
						endID.setRun(false);
						endEX.setRun(false);
					}
				}
			}
			if(EXMEMReg.getInst() == "lw"){
				vector<unsigned char> lwChar;
				for(int i = 0; i < 4; i++){
					lwChar.push_back(bigEndianData[EXMEMReg.getTarget() + i]);
				}
				unsigned int littleData = 0;
				if(lwChar[0] == 0 && lwChar[1] == 0 && lwChar[2] == 0 && lwChar[3] != 0){
					littleData = (unsigned int)lwChar[3];
				} else {
					littleData = ((unsigned int)lwChar[0] << 0) |
								 ((unsigned int)lwChar[1] << 8) |
								 ((unsigned int)lwChar[2] << 16) |
								 ((unsigned int)lwChar[3] << 24);
				}
				endMEM.setALU(littleData);
			}
			if(EXMEMReg.getInst() == "lb"){
				char lbChar = bigEndianData[EXMEMReg.getTarget()];
				int littleData = (int)lbChar;
				endMEM.setALU(littleData);
			}
			if(EXMEMReg.getInst() == "sw"){
				unsigned int tempALU = EXMEMReg.getALU();
				unsigned char* pointer = (unsigned char*)&tempALU;
				unsigned int tar = EXMEMReg.getTarget();
				if(tempALU <= 0xFFFFFFFF && tempALU > 0xFFFFFF){
					for(int j = 0; j < 4; j++){
						bigEndianData[tar + j] = pointer[3-j];
					}
				}
				if(tempALU <= 0xFFFFFF && tempALU > 0xFFFF){
					bigEndianData[tar] = pointer[2];
					bigEndianData[tar + 1] = pointer[1];
					bigEndianData[tar + 2] = pointer[0];
					bigEndianData[tar + 3] = 0;
				}
				if(tempALU <= 0xFFFF && tempALU > 0xFF){
					if(pointer[0] == 0){
						bigEndianData[tar] = pointer[0];
						bigEndianData[tar] = pointer[1];
					} else {
						bigEndianData[tar] = pointer[1];
						bigEndianData[tar + 1] = pointer[0];
					}
					bigEndianData[tar + 2] = 0;
					bigEndianData[tar + 3] = 0;
				}
				if(tempALU <= 0xFF && tempALU != 0){
					bigEndianData[tar] = 0;
					bigEndianData[tar + 1] = 0;
					bigEndianData[tar + 2] = 0;
					bigEndianData[tar + 3] = pointer[0];
				}
				if(tempALU == 0){
					for(int j = 0; j < 4; j++){
						bigEndianData[tar + j] = 0;
					}
				}
				vector<unsigned int> little = bigToLittle(bigEndianData);
				updateDataAddr(little, &address);
			}
			if(EXMEMReg.getInst() == "sb"){
				bigEndianData[EXMEMReg.getTarget()] = EXMEMReg.getALU();
				vector<unsigned int> little = bigToLittle(bigEndianData);
				updateDataAddr(little, &address);
			}
		} else {
			pipeOutPutArray[3] = "";
		}

		// state register update
		if(endIF.getRun()){
			IFIDReg.update(endIF);
		} else {
			IFIDReg.setRun(false);
		}
		if(endID.getRun()){
			IDEXReg.update(endID);
		} else {
			IDEXReg.setRun(false);
		}
		if(endEX.getRun()){
			EXMEMReg.update(endEX);
		} else {
			EXMEMReg.setRd(32); // its not quite answer, temporary
			EXMEMReg.setRun(false);
		}
		if(endMEM.getRun()){
			MEMWBReg.update(endMEM);
		} else {
			MEMWBReg.setRun(false);
		}

		// hazard detection
		// data hazard
		// EX/MEM to EX forward
		if(EXMEMReg.getRW() && (EXMEMReg.getRd() != 0) && (EXMEMReg.getRd() == IDEXReg.getRs()) && IDEXReg.getRun() && EXMEMReg.getRun()){
			IDEXReg.setRsData(EXMEMReg.getALU());
		}
		if(EXMEMReg.getRW() && (EXMEMReg.getRd() != 0) && (EXMEMReg.getRd() == IDEXReg.getRt()) && IDEXReg.getRun() && EXMEMReg.getRun()){
			IDEXReg.setRtData(EXMEMReg.getALU());
		}
		// MEM/WB to EX forward
		if(MEMWBReg.getRW() && (MEMWBReg.getRd() != 0) && (EXMEMReg.getRd() != IDEXReg.getRs()) && (MEMWBReg.getRd() == IDEXReg.getRs()) && MEMWBReg.getRun() && IDEXReg.getRun()){
			IDEXReg.setRsData(MEMWBReg.getALU());
		}
		if(MEMWBReg.getRW() && (MEMWBReg.getRd() != 0) && (EXMEMReg.getRd() != IDEXReg.getRt()) && (MEMWBReg.getRd() == IDEXReg.getRt()) && MEMWBReg.getRun() && IDEXReg.getRun()){
			IDEXReg.setRtData(MEMWBReg.getALU());
		}
		// load-use
		if(IDEXReg.getMR() && IDEXReg.getRun() && ((IDEXReg.getRt() == IFIDReg.getRs()) || (IDEXReg.getRt() == IFIDReg.getRt()))){
			// store right after load : avoid stall
			if(checkOp(IFIDReg.getInst()) == 43 || checkOp(IFIDReg.getInst()) == 40);	// sw, sb
			else {
				pcWrite = false;
				IFIDReg.setRun(false);
			}
		}
		// MEM/WB to MEM forward
		if(MEMWBReg.getMR() && MEMWBReg.getRun() && EXMEMReg.getRun() && EXMEMReg.getMW() && ((MEMWBReg.getRt() == EXMEMReg.getRt()))){
			EXMEMReg.setALU(MEMWBReg.getALU());
		}
		// control hazard is implemented in pipeline code
		
		int nextPc;
		// if jump, branch, dont plus 4
		if(pcChange){
			currPc += 4;
			nextPc = endIF.getNPC();
		}
		if(pcChange == false){
			nextPc = currPc;
		}
		totalCycle+=1;

		printOutput(nextPc, regs, mAddrs, address, totalCycle, pipeOutPutArray, false, showPipe, afterProcess);
		if(whileExit){
			if(isLimit == false){
				pipeOutPutArray[4] = "";
			}
			printOutput(endIF.getNPC(), regs, mAddrs, address, totalCycle, pipeOutPutArray, true, true, true);
			break;
		}
	}
		return 0;	
}
