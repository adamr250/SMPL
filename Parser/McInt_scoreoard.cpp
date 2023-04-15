#include "McFunction.h"



std::string McFunction::McAddInt(std::string a) {
	std::string Out = "";
	if(this->baseIntTabs) {
		Out += "scoreboard players set " + a + " SMPL_Int 0\n";
	}else {
		this->baseIntTabs = true;
		Out += McAddIntTab("SMPL_Int");
		Out += McAddIntTab("SMPL_Tmp");
		Out += "scoreboard players set " + a + " SMPL_Int 0\n";
	}
	return Out;
}
std::string McFunction::McAddIntTab(std::string a) {
	std::string Out = "";
	this->varTabIntList.push_back(a);
	Out += "scoreboard objectives add " + a + " dummy\n";
	return Out;
} 


std::string McFunction::McOperationInt(Statement comm) {
	std::string Out = "";
	std::string tmp1 = "";
	std::string tmp2 = "";
	bool SetBackToArray = false;
	std::string SetBackToArrayName = "";
	
	if(IfArray(comm.argv[0])) {
		SetBackToArray = true;
		SetBackToArrayName = (comm.argv[0]);
		tmp1 = GetArrayName(comm.argv[0]);
		tmp2 = GetArrayIndex(comm.argv[0]);
		Out += McGetValueFromIntArray(tmp1,tmp2);
		comm.argv[0] = "<array" + tmp1 + tmp2 + ">";
	}
	if(IfArray(comm.argv[1])) {
		tmp1 = GetArrayName(comm.argv[1]);
		tmp2 = GetArrayIndex(comm.argv[1]);
		Out += McGetValueFromIntArray(tmp1,tmp2);
		comm.argv[1] = "<array" + tmp1 + tmp2 + ">";
	}
	for (int i = 0;i < comm.argc; i++) {
		if(!(isNumber(comm.argv[i]))) {
			if(comm.argv[i][0] == '<' && comm.argv[i][comm.argv[i].length()-1] == '>') {
				comm.argv[i] += " SMPL_Tmp";
			}else {
				comm.argv[i] += " SMPL_Int";
			}
		}

	}
	
	
	if(comm.name == "=") {
		if(comm.argc == 2) {
			Out += McIntSet(comm.argv[0],comm.argv[1]);
		}
	}else if(comm.name == "+=") {
		if(comm.argc == 2) {
			Out += McIntAddEqual(comm.argv[0],comm.argv[1]);
		}
	}else if(comm.name == "-=") {
		if(comm.argc == 2) {
			Out += McIntSubtractEqual(comm.argv[0],comm.argv[1]);
		}
	}else if(comm.name == "*=") {
		if(comm.argc == 2) {
			Out += McIntMultiplyEqual(comm.argv[0],comm.argv[1]);
		}
	}else if(comm.name == "/=") {
		if(comm.argc == 2) {
			Out += McIntDivideEqual(comm.argv[0],comm.argv[1]);
		}
	}else if(comm.name == "%=") {
		if(comm.argc == 2) {
			Out += McIntModuloEqual(comm.argv[0],comm.argv[1]);
		}
	}/*else if(comm.name == "swap") {
		if(comm.argc == 2) {
			Out += McIntAddEqual(comm.argv[0],comm.argv[1]);
		}
	}*/
	
	
	if(SetBackToArray) {
		tmp1 = GetArrayName(SetBackToArrayName);
		tmp2 = GetArrayIndex(SetBackToArrayName);
		Out += McSetValueOnIntArray(tmp1,tmp2,comm.argv[0]);
	}
	
	return Out;
} 



std::string McFunction::McIntSet(std::string result, std::string a) {
	std::string Out = "";
	if(isNumber(a)) {
		Out += "scoreboard players set " + result + " " + a + "\n";
	}else {
		Out += "scoreboard players operation " + result + " = " + a + "\n";
	}
	return Out;
}
std::string McFunction::McIntAddEqual(std::string result, std::string a) {
	std::string Out = "";

	if(isNumber(a)) {
		Out += "scoreboard players set <operationTmp> SMPL_Tmp " + a + "\n";
		Out += "scoreboard players operation " + result + " += <operationTmp> SMPL_Tmp\n";
	}else {
		Out += "scoreboard players operation " + result + " += " + a + "\n";
	}
	return Out;
}

std::string McFunction::McIntSubtractEqual(std::string result, std::string a) {
	std::string Out = "";

	if(isNumber(a)) {
		Out += "scoreboard players set <operationTmp> SMPL_Tmp " + a + "\n";
		Out += "scoreboard players operation " + result + " -= <operationTmp> SMPL_Tmp\n";
	}else {
		Out += "scoreboard players operation " + result + " -= " + a + "\n";
	}
	return Out;
}

std::string McFunction::McIntMultiplyEqual(std::string result, std::string a) {
	std::string Out = "";

	if(isNumber(a)) {
		Out += "scoreboard players set <operationTmp> SMPL_Tmp " + a + "\n";
		Out += "scoreboard players operation " + result + " *= <operationTmp> SMPL_Tmp\n";
	}else {
		Out += "scoreboard players operation " + result + " *= " + a + "\n";
	}

	return Out;
}

std::string McFunction::McIntDivideEqual(std::string result, std::string a) {
	std::string Out = "";

	if(isNumber(a)) {
		Out += "scoreboard players set <operationTmp> SMPL_Tmp " + a + "\n";
		Out += "scoreboard players operation " + result + " /= <operationTmp> SMPL_Tmp\n";
	}else {
		Out += "scoreboard players operation " + result + " /= " + a + "\n";
	}

	return Out;
}

std::string McFunction::McIntModuloEqual(std::string result, std::string a) {
	std::string Out = "";

	if(isNumber(a)) {
		Out += "scoreboard players set <operationTmp> SMPL_Tmp " + a + "\n";
		Out += "scoreboard players operation " + result + " %= <operationTmp> SMPL_Tmp\n";
	}else {
		Out += "scoreboard players operation " + result + " %= " + a + "\n";
	}

	return Out;
}

std::string McFunction::McIntSwap(std::string a, std::string b) {
	std::string Out = "";
	Out += "scoreboard players operation " + a + " >< " + b + "\n";
	return Out;
}

