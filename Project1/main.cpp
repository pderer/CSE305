#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <bitset>
#include <sstream>

using namespace std;

map<string, unsigned int> dataLabel;
const char* zero = "0";
const char* one = "1";
const char* x = "x";

// Part 4.

string oneReg(string s){
	s.erase(0,1);
	s.pop_back();
	int firstNum = stoi(s);
	bitset<5> bn1(firstNum);
	string s1 = bn1.to_string();
	return s1;
}

string oneRegImm(vector<string> v){
	string firstReg = *(v.begin());
	string var = *(v.begin()+1);
	firstReg.erase(0,1);
	firstReg.pop_back();
	int firstNum = stoi(firstReg);
	bitset<5> bn1(firstNum);
	string s1 = bn1.to_string();
	
	if(var[0] == *zero && var[1] == *x){
                string hexSt = var;
                const int decNum = stoi(hexSt, 0, 16);
                bitset<16> bn2(decNum);
                string s2 = bn2.to_string();
                s1.append(s2);
        } else if(dataLabel.count(var)){
                auto item = dataLabel.find(var);
                bitset<16> addrBi(item->second);
                string addrSt = addrBi.to_string();
		s1.append(addrSt);
        } else {
        int secondNum = stoi(var);
        bitset<16> bn2(secondNum);
        string s2 = bn2.to_string();
        s1.append(s2);
        }
	return s1;
}

string oneRegAddr(vector<string> v){
	string firstReg = *(v.begin());
	string addr = *(v.begin()+1);
	firstReg.erase(0,1);
	firstReg.pop_back();
	int firstNum = stoi(firstReg);
	bitset<5> bn1(firstNum);
	string s1 = bn1.to_string();

	const char* open = "(";
	string offset;
	string rs;
	for(auto i = addr.begin(); i != addr.end(); i++){
		if(*i != *open){
			offset.push_back(*i);
		} else if (*i == *open) {
			for(auto j = i+2; j != addr.end()-1; j++){
				rs.push_back(*j);
			}
			break;
		}
	}
	int secondNum = stoi(rs);
	bitset<5> bn2(secondNum);
	string s2 = bn2.to_string();
	s2.append(s1);
	if(offset[0] == *zero && offset[1] == *x){
		const int decNum = stoi(offset, 0, 16);
		bitset<16> bn3(decNum);
		string s3 = bn3.to_string();
		s2.append(s3);
	} else {
		int thirdNum = stoi(offset);
	        bitset<16> bn3(thirdNum);
        	string s3 = bn3.to_string();
        	s2.append(s3);

	}
	return s2;
}

string twoRegImm(vector<string> v){
        string firstReg = *(v.begin());
        string secondReg = *(v.begin()+1);
	string var = *(v.begin()+2);
        firstReg.erase(0,1);
        firstReg.pop_back();
        secondReg.erase(0,1);
        secondReg.pop_back();
        int firstNum = stoi(firstReg);
        int secondNum = stoi(secondReg);

        bitset<5> bn1(firstNum);
        bitset<5> bn2(secondNum);

	string s1, s2;
        s1 = bn1.to_string();
        s2 = bn2.to_string();
	string result;
	result.append(s2).append(s1); //rt, rs sequence
	
	if(var[0] == *zero && var[1] == *x){
		const int decNum = stoi(var, 0, 16);
		bitset<16> bn3(decNum);
		string s3 = bn3.to_string();
		result.append(s3);
	} else {
		int thirdNum = stoi(var);
        	bitset<16> bn3(thirdNum);
		string s3 = bn3.to_string();
		result.append(s3);
		
	}
        return result;
}

string twoReg(vector<string> v){
	string firstReg = *(v.begin());
        string secondReg = *(v.begin()+1);
	firstReg.erase(0,1);
        firstReg.pop_back();
        secondReg.erase(0,1);
        secondReg.pop_back();
        int firstNum = stoi(firstReg);
        int secondNum = stoi(secondReg);

        bitset<5> bn1(firstNum);
        bitset<5> bn2(secondNum);

        string s1, s2;
        s1 = bn1.to_string();
        s2 = bn2.to_string();
        string result;
        result.append(s1).append(s2); //rs, rt sequence
	return result;
}


string threeReg(vector<string> v){
	string firstReg = *(v.begin());
        string secondReg = *(v.begin()+1);
	string thirdReg = *(v.begin()+2);
        firstReg.erase(0,1);
        firstReg.pop_back();
        secondReg.erase(0,1);
        secondReg.pop_back();
	thirdReg.erase(0,1);
        int firstNum = stoi(firstReg);
        int secondNum = stoi(secondReg);
	int thirdNum = stoi(thirdReg);

	bitset<5> bn1(firstNum);
        bitset<5> bn2(secondNum);
	bitset<5> bn3(thirdNum);

	string s1, s2, s3;
        s1 = bn1.to_string();
        s2 = bn2.to_string();
	s3 = bn3.to_string();
	string result;
	result.append(s2).append(s3).append(s1);

	return result;
}

string twoRegShift(vector<string> v){
	string firstReg = *(v.begin());
        string secondReg = *(v.begin()+1);
        string shift = *(v.begin()+2);
        firstReg.erase(0,1);
        firstReg.pop_back();
        secondReg.erase(0,1);
        secondReg.pop_back();
        int firstNum = stoi(firstReg);
        int secondNum = stoi(secondReg);

        bitset<5> bn1(firstNum);
        bitset<5> bn2(secondNum);

        string s1 = bn1.to_string();
        string s2 = bn2.to_string();
        string result;
        result.append(s2).append(s1); //rt, rd sequence

        if(shift[0] == *zero && shift[1] == *x){
                const int decNum = stoi(shift, 0, 16);
                bitset<5> bn3(decNum);
                string s3 = bn3.to_string();
                result.append(s3);
        } else {
        int thirdNum = stoi(shift);
        bitset<5> bn3(thirdNum);
        string s3 = bn3.to_string();
        result.append(s3);
        }
        return result;

}

// Part 7-1.

string outputSize(vector<unsigned int> v){
        stringstream ss;
        ss << "0x" << hex << 4* v.size();
        string result = ss.str();
        return result;
        }
string outputAddr(vector<unsigned int> v){
        stringstream ss;
        for(int i = 0; i < v.size(); i++){
                ss << "0x" << hex << v[i] << "\n";
        }
        string result = ss.str();
        return result;
        }


int main(int argc, char* argv[])
{
	vector<string> words;
	string word;

	vector<string> datas;
	vector<string> texts;
	
	const char* colon(":");

	map<string, unsigned int> textLabel;
	
	vector<string> variables;
	vector<string> instructions;

	unsigned int addr = 4194304;
	unsigned int addr2 = 268435456;

	// Part 1.

	ifstream ifas(argv[1]);

	if(!ifas)
	{
		cerr<<"could not open this file"<<'\n';
		return 1;
	}

	while(ifas >> word)
	{
		words.push_back(word);
	}
	
	auto dataAddr = find(words.begin(), words.end(), ".data");
	auto textAddr = find(words.begin(), words.end(), ".text");
	
	for (auto i = dataAddr+1; i < textAddr; i ++)
		datas.push_back(*i);
	
	for(auto i = textAddr+1; i < words.end(); i++)
		texts.push_back(*i);	
	
	// Part 2.

	for(auto i = datas.begin(); i < datas.end(); i++){
		string buffer(*i);
		string check;
		if(i > datas.begin()){
			check = *(i-1);
		}
		bool isLabel = false;
		if(check.back() == *colon){
			isLabel = true;
		}
		if(buffer == ".word"){
			string var = *(i+1);
			if(var[0] == *zero && var[1] == *x){
				const int decNum = stoi(var, 0, 16);
				bitset<32> bn1(decNum);
				string s1 = bn1.to_string();
				variables.push_back(s1);
			} else {
				int num = stoi(var);
				bitset<32> bn1(num);
				string s1 = bn1.to_string();
				variables.push_back(s1);
			}
			if(isLabel == true){
				check.pop_back();
				dataLabel.insert(pair<string, unsigned int>(check, addr2));
			}
			addr2 = addr2 + 4;
		}
	}

	// Part 3.

	for(auto i = texts.begin(); i < texts.end(); i++){
		string buffer(*i);
		string check;
		if(i>texts.begin()){
			check = *(i-1);
		}
		bool isLabel = false;
		if(check.back() == *colon){
			//textLabel.insert(pair<string, string>(*i, *(i+1)));
			isLabel = true;
		}
		if(buffer == "addiu"){
			string op = "001001";
			vector<string> v;
			v.push_back(*(i+1));
			v.push_back(*(i+2));
			v.push_back(*(i+3));
			string preSt = twoRegImm(v);
			op.append(preSt);
			instructions.push_back(op);
			if(isLabel == true){
				textLabel.insert(pair<string, unsigned int>(check, addr));
			}
			addr = addr + 4;
		}
		if(buffer == "addu"){
			string op = "000000";
			string shamt = "00000";
			string funct = "0x21";
			vector<string> v;
                	v.push_back(*(i+1));
        	        v.push_back(*(i+2));
	                v.push_back(*(i+3));
			const int decNum = stoi(funct, 0, 16);
        		bitset<6> bn(decNum);
       			string functBi = bn.to_string();
       			string preSt = threeReg(v);
       			op.append(preSt).append(shamt).append(functBi);
      			instructions.push_back(op);
			if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }

	                addr = addr + 4;
		}

		if(buffer == "and"){
			string op = "000000";
			string shamt = "00000";
			string funct = "0x24";
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        const int decNum = stoi(funct, 0, 16);
                        bitset<6> bn(decNum);
                        string functBi = bn.to_string();
                        string preSt = threeReg(v);
                        op.append(preSt).append(shamt).append(functBi);
                        instructions.push_back(op);
			if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "andi"){
			string op = "001100";
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        string preSt = twoRegImm(v);
                        op.append(preSt);
                        instructions.push_back(op);
			if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		// Part 5-1.
		if(buffer == "beq"){
			string op = "000100";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
			string labelKey = *(i+3);
			labelKey.append(":");
                        string preSt = twoReg(v);
                        op.append(preSt);
			if(textLabel.count(labelKey)){
                                auto item = textLabel.find(labelKey);
				unsigned int offsetNum = (item->second)/4 - (addr/4) - 1;
                                bitset<16> addrBi(offsetNum); 
                                string addrSt = addrBi.to_string();
			 	op.append(addrSt);
                        } else {
				unsigned int undefAddr = addr;
				string undefSt = to_string(undefAddr);
				undefSt.append(":");
				op.append(labelKey).append(undefSt);	
			}	
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;

		}
		// Part 5-2.
		if(buffer == "bne"){
			string op ="000101";
			vector<string> v;
			v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string labelKey = *(i+3);
                        labelKey.append(":");
                        string preSt = twoReg(v);
                        op.append(preSt);
                        if(textLabel.count(labelKey)){
                                auto item = textLabel.find(labelKey);
                                unsigned int offsetNum = (item->second)/4 - (addr/4) - 1;
                                bitset<16> addrBi(offsetNum);
                                string addrSt = addrBi.to_string();
                                op.append(addrSt);
                        } else {
				unsigned int undefAddr = addr;
                                string undefSt = to_string(undefAddr);
				undefSt.append(":");
                                op.append(labelKey).append(undefSt);
                        }
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "j"){
			string op = "000010";
			string labelKey = *(i+1);
			labelKey.append(":");
			if(textLabel.count(labelKey)){
				auto item = textLabel.find(labelKey);
				bitset<26> addrBi((item->second)/4);
				string addrSt = addrBi.to_string();
				op.append(addrSt);
			} else {
				op.append(labelKey);
			}
			instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "jal"){
			string op = "000011";
			string labelKey = *(i+1);
                        labelKey.append(":");
                        if(textLabel.count(labelKey)){
                                auto item = textLabel.find(labelKey);
                                bitset<26> addrBi((item->second)/4);
                                string addrSt = addrBi.to_string();
                                op.append(addrSt);
                        } else {
                                op.append(labelKey);
                        }
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "jr"){
			string op = "000000";
			string s = *(i+1);
			string rs = oneReg(s);
			op.append(rs);
			bitset<15> bn(0);
			string s0 = bn.to_string();
			op.append(s0);
			bitset<6> bn2(8);
			string s1 = bn2.to_string();
			op.append(s1);
			instructions.push_back(op);
			if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
			addr = addr + 4;
		}
		if(buffer == "lui"){
			string op = "001111";
			bitset<5> bn1(0);
			op.append(bn1.to_string());
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string preSt = oneRegImm(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "lw"){
			string op ="100011";
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string preSt = oneRegAddr(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;			
		}
		if(buffer == "lb"){
			string op = "100000";
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string preSt = oneRegAddr(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "nor"){
			string op = "000000";
                        string shamt = "00000";
                        string funct = "0x27";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        const int decNum = stoi(funct, 0, 16);
                        bitset<6> bn(decNum);
                        string functBi = bn.to_string();
                        string preSt = threeReg(v);
                        op.append(preSt).append(shamt).append(functBi);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "or"){
			string op = "000000";
                        string shamt = "00000";
                        string funct = "0x25";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        const int decNum = stoi(funct, 0, 16);
                        bitset<6> bn(decNum);
                        string functBi = bn.to_string();
                        string preSt = threeReg(v);
                        op.append(preSt).append(shamt).append(functBi);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "ori"){
			string op = "001101";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        string preSt = twoRegImm(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "sltiu"){
			string op = "001011";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        string preSt = twoRegImm(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "sltu"){
			string op = "000000";
                        string shamt = "00000";
                        string funct = "0x2b";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        const int decNum = stoi(funct, 0, 16);
                        bitset<6> bn(decNum);
                        string functBi = bn.to_string();
                        string preSt = threeReg(v);
                        op.append(preSt).append(shamt).append(functBi);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "sll"){
			string op = "00000000000";
			string funct = "000000";
			vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
			string preSt = twoRegShift(v);
			op.append(preSt).append(funct);
			instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;	
		}
		if(buffer == "srl"){
			string op = "00000000000";
                        string funct = "000010";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        string preSt = twoRegShift(v);
                        op.append(preSt).append(funct);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "sw"){
			string op = "101011";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string preSt = oneRegAddr(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "sb"){
			string op = "101000";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        string preSt = oneRegAddr(v);
                        op.append(preSt);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		if(buffer == "subu"){
			string op = "000000";
                        string shamt = "00000";
                        string funct = "0x23";
                        vector<string> v;
                        v.push_back(*(i+1));
                        v.push_back(*(i+2));
                        v.push_back(*(i+3));
                        const int decNum = stoi(funct, 0, 16);
                        bitset<6> bn(decNum);
                        string functBi = bn.to_string();
                        string preSt = threeReg(v);
                        op.append(preSt).append(shamt).append(functBi);
                        instructions.push_back(op);
                        if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}
		// Part 5-3.
		if(buffer == "la"){
			string opLui = "001111";
			string opOri = "001101";
			string rsLui = "00000";
			string reg = *(i+1);
			reg.erase(0,1);
			reg.pop_back();
			int regNum = stoi(reg);
			bitset<5> bnRt(regNum);
			string rt = bnRt.to_string();
			string varAddr = *(i+2);
			auto item = dataLabel.find(varAddr);
			bitset<32> bn(item->second);			
			string s1 = bn.to_string();
			string luiResult;
			string oriResult;
			bool checkZero = true;
			for(int i = 16; i < s1.size(); i++){
				if(s1[i] = *zero){
					continue;
				} else {
					checkZero = false;
					break;
				}
			}
			string luiImm;
                        for(int i = 0; i < 16; i++){
                                luiImm.push_back(s1[i]);
                        }
                        luiResult.append(opLui).append(rsLui).append(rt).append(luiImm);
                        instructions.push_back(luiResult);

			if(checkZero == false){
				string oriImm;
				for(int i = 16; i < s1.size(); i++){
					oriImm.push_back(s1[i]);
				}
				oriResult.append(opOri).append(rt).append(rt).append(oriImm);
				instructions.push_back(oriResult);
			}
			if(isLabel == true){
                                textLabel.insert(pair<string, unsigned int>(check, addr));
                        }
                        addr = addr + 4;
		}

	}
	
	// Part 6.

	for(int i = 0; i < instructions.size(); i++){
		if(instructions[i].back() == *colon){
			string insText;
			for(int j = 0; j < instructions[i].size(); j++){
				if(instructions[i][j] != *zero && instructions[i][j] != *one){
					for(int k = j; k < instructions[i].size(); k++){
						insText.push_back(instructions[i][k]);
					}
					instructions[i].erase(j, instructions[i].length()-j);
					break;
				}
			}
			if(instructions[i].length() == 6){
				if(insText == "exit:"){
					int exitAddr = addr + 4;
					bitset<26> addrBi(exitAddr/4);
					string addrSt = addrBi.to_string();
					instructions[i].append(addrSt);
				} else {
					auto item = textLabel.find(insText);
					bitset<26> addrBi((item->second)/4);
					string addrSt = addrBi.to_string();
					instructions[i].append(addrSt);
				}
			} else {
				string undefAddr;
				for(int i = 0; i < insText.size(); i++){
                                        if(insText[i] == *colon){
                                                for(int j = i+1; j < insText.size(); j++){
                                                        undefAddr.push_back(insText[j]);
                                                }
						insText.erase(i+1, insText.length()-i-1);
						break;
                                        }	
                               	 }
				auto item = textLabel.find(insText);
				undefAddr.pop_back();
				unsigned long undefInt = stoul(undefAddr);
				unsigned int offsetNum = (item->second)/4 - (undefInt/4) - 1;
				bitset<16> addrBi(offsetNum);
				string addrSt = addrBi.to_string();
				instructions[i].append(addrSt); 
			}
		}
	}
	
	// test in console
	/*
	for(auto i = instructions.begin(); i< instructions.end(); i++){
                        cout<< *i<<endl;
                }
	for(auto i =textLabel.begin(); i!=textLabel.end(); i++){
		cout<<i->first<<i->second<<endl;
	}
	for(auto i = dataLabel.begin(); i!=dataLabel.end(); i++){
		cout<<i->first<<i->second<<endl;
	}
	for(auto i = variables.begin(); i!= variables.end(); i++){
			cout<< *i <<endl;
		}
	*/

	// Part 7-2.	
	vector<unsigned int> instructionsInt;
	vector<unsigned int> variablesInt;

	for(int i = 0 ; i < instructions.size(); i++){
		bitset<32> bn(instructions[i]);
		unsigned long dec = bn.to_ulong();
		instructionsInt.push_back(dec);
	}

	for(int i = 0; i < variables.size(); i++){
		bitset<32> bn(variables[i]);
		unsigned long dec = bn.to_ulong();
                variablesInt.push_back(dec);
        }

	string outputText(argv[1]);
	outputText.pop_back();
	outputText.append("o");
	ofstream outf{outputText};
	if (!outf){
		cout<<"could not be opend\n";
	}
	outf << outputSize(instructionsInt) << "\n" << outputSize(variablesInt) << "\n" << outputAddr(instructionsInt) << outputAddr(variablesInt);

	return 0;
}
