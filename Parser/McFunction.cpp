#include "McFunction.h"
#include "../Names.h"
#include <ctype.h>
#include <fstream>
#include <set>

McFunction::McFunction() {
	this->Name = "";
	this->Content = "";
	this->baseIntTabs = false;	
}
McFunction::McFunction(const McFunction &mcfunction){
	this->Name = mcfunction.Name;
	this->Content = mcfunction.Content;
	this->varIntList = mcfunction.varIntList;
	this->varTabIntList = mcfunction.varTabIntList;
	this->baseIntTabs = mcfunction.baseIntTabs; 
	this->functions = mcfunction.functions;
	this->inputStat = mcfunction.inputStat;
}
void McFunction::operator=(const McFunction &mcfunction) {
	this->Name = mcfunction.Name;
	this->Content = mcfunction.Content;
	this->baseIntTabs = mcfunction.baseIntTabs; 
	this->varIntList.clear();
	this->varIntList.clear();
	this->varIntList = mcfunction.varIntList;
	this->varTabIntList = mcfunction.varTabIntList;
	this->functions = mcfunction.functions;
	this->inputStat = mcfunction.inputStat;
}

std::string McFunction::getName() {
	return Name;
}

bool McFunction::isNumber(std::string word) {
		int dummy_int;
		int scan_value = std::sscanf( word.c_str(), "%d", &dummy_int);
		return scan_value;
}
bool McFunction::isCmpOperator(std::string word) {
		return (word=="!=" || word=="==" || word=="<=" || word=="=>" || word=="<" || word==">");
}
bool McFunction::IfArray(std::string a) {
	if(a.find('[') != std::string::npos && a.find(']') != std::string::npos) return 1;
	return 0;
}
void McFunction::newContent() {
	this->Content = "";
}
void McFunction::addContent(std::string content) {
	this->Content += content;
}
void McFunction::Rename(std::string name) {
	Name = name;
}
void McFunction::RenameAndNewContent(std::string name) {
	this->Content = "";
	this->Name = name;
}
void McFunction::CopyAndRename(std::string name,McFunction dad) {
	this->varIntList = dad.varIntList;
	this->varTabIntList = dad.varTabIntList;
	this->baseIntTabs = dad.baseIntTabs;
}
void McFunction::setVarsLists(std::vector <std::string> IntList, std::vector <std::string> TabIntList) {
	this->varIntList = IntList;
	this->varTabIntList = TabIntList;
}
void McFunction::setOutsideFunctions (std::vector <OutsideFunction> functions) {
	this->functions = functions;
}


void McFunction::clean() {
	for(int i = 0; i < (int)varTabIntList.size(); i++) {
		this->Content += "scoreboard objectives remove " + varTabIntList[i] + "\n";
	}
	if(this->IntArrayPointer) {
		this->Content += "kill @e[type=armor_stand,tag=SMPL_ArrayPointer]\n";
	}
	if(this->MaxArraySize > 0) {
		this->Content += "kill @e[type=armor_stand,tag=SMPL_ArrayElement]\n";
	}
}

void McFunction::SaveFunction(std::string dir) {
	//std::cout<<inputStat.name<<std::endl;
	if(inputStat.name=="for") {
		parseStatement(inputStat.statv[2]);
		Content += parseIf(inputStat.statv[1]);
	}
	else if(inputStat.name=="while") {
		Content += parseIf(inputStat);
	}
	std::ofstream file;
	file.open(dir + Name + ".mcfunction", std::ios::out);
		file << Content;
	file.close();
}

std::string McFunction::parseSay(Statement stat) {
	std::string Out = "";
	
	// Call McGetValueFromIntArray exactly once for every occurring array element
	std::set <std::string> array_elements;
	for(int i = 0; i<stat.argc; i++) {
		if(IfArray(stat.argv[i]) && array_elements.count(stat.argv[i])==0){
			Out += McGetValueFromIntArray(GetArrayName(stat.argv[i]), GetArrayIndex(stat.argv[i]));
			array_elements.insert(stat.argv[i]);
		}
	}
	
	Out += "tellraw @a [";
	for(int i = 0; i<stat.argc; i++) {
		if(stat.argv[i][0]=='"'){ // string literal
			Out += "{\"text\":" + stat.argv[i] + "}"; 
		}
		else if(IfArray(stat.argv[i])){ // array element
			std::string temp_variable = "<array" + GetArrayName(stat.argv[i]) + GetArrayIndex(stat.argv[i]) + ">";
			Out += "{\"score\":{\"name\":\"" + temp_variable + "\",\"objective\":\"SMPL_Tmp\"}}";
		}
		else{ // variable
			Out += "{\"score\":{\"name\":\"" + stat.argv[i] + "\",\"objective\":\"SMPL_Int\"}}";
		}
		if(i<stat.argc-1) Out += ",";
	}
	Out += "]\n";
	return Out;
}

std::string McFunction::parseScoreCondition(Statement stat) {
	std::string Out = "";
	if(stat.argc<3) return Out;
	
	std::string op = stat.argv[1];
	if(op=="==") op = "=";
	
	if(isNumber(stat.argv[0])) { // swap args and reverse the operator if a literal was first
		std::swap(stat.argv[0], stat.argv[2]);
		if(op=="<=") op = ">=";
		else if(op==">=") op = "<=";
		else if(op=="<") op = ">";
		else if(op==">") op = "<";
	}
	
	Out += "score " + stat.argv[0] + " SMPL_Int ";
	
	if(isNumber(stat.argv[2])) { // compare to literal
		Out += "matches ";
		if(op=="=") Out += stat.argv[2];
		else if(op=="<=") Out += ".." + stat.argv[2];
		else if(op==">=") Out += stat.argv[2] + "..";
		else if(op=="<") Out += ".." + std::to_string(std::stoi(stat.argv[2])-1);
		else if(op==">") Out += std::to_string(std::stoi(stat.argv[2])+1) + "..";
	}
	else { // compare to variable
		Out += op + " " + stat.argv[2] + " SMPL_Int";
	}
	
	return Out;
}

std::string McFunction::parseIf(Statement stat) {
	std::string Out = "";
	if(stat.argc<2) return Out;
	
	std::string subcomm;
	if(stat.name=="unless") subcomm = "unless";
	else subcomm = "if";
	Out += "execute " + subcomm + " ";
	
	if(stat.argv[0][0]=='@') Out += "entity " + stat.argv[0]; // entity
	else if(isCmpOperator(stat.argv[1])) Out += parseScoreCondition(stat); // score
	else if(stat.argc>=4) Out += "block " + stat.argv[0] + " " + stat.argv[1] + " " + stat.argv[2] + " #" + stat.argv[3]; // block
	
	Out += " run " + McRun(stat.argv[stat.argc-2], stat.argv[stat.argc-1]);
	return Out;
}

std::string McFunction::parseModifySubcommand(Statement stat) {
	std::string Out = "";
	if(stat.argc<2) return Out;
	
	Out += "execute " + stat.name + " ";
	for(int i = 0; i<stat.argc-2; i++) {
		Out += stat.argv[i] + " ";
	}
	Out += "run " + McRun(stat.argv[stat.argc-2], stat.argv[stat.argc-1]);
	return Out;
}

int McFunction::parseStatement(Statement comm) {
	std::string Out = "";
	if(comm.argc != (int)comm.argv.size()) return -1;
	
	if(comm.name == "say"){
		Out += parseSay(comm);
	}else if(comm.name == "if" || comm.name == "while"){
		Out += parseIf(comm);
	}else if(comm.name == "for"){
		parseStatement(comm.statv[0]);
		Out += parseIf(comm.statv[1]);
	}else if(comm.name == "as" || comm.name == "at" || comm.name == "positioned"){
		Out += parseModifySubcommand(comm);
	}else if(comm.name == "int") {
		if(comm.argc == 1) {
			if (IfArray(comm.argv[0])) {
				Out += McCreateNewIntArray(comm.argv[0]);
			}else {
				Out += McAddInt(comm.argv[0]);
			}
		}
	}else if(
		comm.name == "=" ||
		comm.name == "swap" ||
		comm.name == "+=" ||
		comm.name == "-=" ||
		comm.name == "*=" ||
		comm.name == "/=" 
	) {
		if(comm.argc != 2) return -1;
		Out += McOperationInt(comm);
	}else if(comm.name == "run") {
		if(comm.argc != 2) return -1;
		Out += McRun(comm.argv[0],comm.argv[1]); 
	}else if(comm.name == "ExpandArrayByOne") {//only temporary
		if(comm.argc != 1) return -1;
		Out += McExpandIntArrayByOne(comm.argv[0]); 
	}else if(comm.name == "push") {//only temporary
		if(comm.argc != 2) return -1;
		Out += McPushValueToIntArray(comm.argv[0],comm.argv[1]); 
	}else {
		bool found = false;
		for (OutsideFunction outsideFunction : this->functions) {
			if(comm.name == outsideFunction.Name) {
				if(comm.argc > outsideFunction.InputArgs) return -1;
				for(int i = 0; i < comm.argc; i ++) {
					if(outsideFunction.InputArgsTypes[i] == 1) {
						if(isNumber(comm.argv[i])) {
							"scoreboard players set <OtherFunctionTmpArg" + std::to_string(i) + "> SMPL_Tmp " + comm.argv[i] + "\n";
							comm.argv[i] = "<OtherFunctionTmpArg" + std::to_string(i) + "> SMPL_Tmp ";
						}else if(IfArray(comm.argv[i])) {
							std::string ArrayName = GetArrayName(comm.argv[i]);
							std::string ArrayIndex = GetArrayIndex(comm.argv[i]);
							Out += McGetValueFromIntArray(ArrayName,ArrayIndex);
							comm.argv[i] = "<array" + ArrayName + ArrayIndex + "> SMPL_Tmp";
						}else if(comm.argv[i].find(' ') == std::string::npos) {
							if(comm.argv[i][0] == '<' && comm.argv[i][comm.argv[i].length()-1] == '>') {
								comm.argv[i] += " SMPL_Tmp";
							}else {
								comm.argv[i] += " SMPL_Int";
							}
						}else {
							std::cout << "OutsideFunction InputArgsTypes 1 Error\n";
							return -1;
						}
							
					}else if(outsideFunction.InputArgsTypes[i] == 5) {
						if(comm.argv[i][0] == '\"' && comm.argv[i][comm.argv[i].length()-1] == '\"') {
							comm.argv[i].erase(comm.argv[i].begin());
							comm.argv[i].erase(comm.argv[i].end()-1);
						}else {
							std::cout << "OutsideFunction InputArgsTypes 5 Error\n";
							return -1;
						}
					}
				}
				
				std::string tmp = "";
				tmp = outsideFunction.parseStatement(comm);
				if(tmp == "-1") {std::cout << "OutsideFunction ParseStatement Fail\n";return -1; }
				
				Out += tmp;
				Out += '\n';
				found = true;
				break;
			}
		}
		if(!found) return -1;
	}
	
	Content += Out;
	//std::cout<<"saving to: "<<Name<<std::endl;
	//std::cout<<Out<<std::endl;
	return 0;
}
