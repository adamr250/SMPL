 #ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <iostream>
#include <string>
#include <vector>
#include "../Statement.h"
#include "OutsideFunction.h"

class McFunction {
public:
	Statement inputStat;

protected:
	std::string Name = "";
	std::string Content = "";
	bool baseIntTabs = false;
	bool IntArrayIndex = false;
	bool IntArrayPointer = false;
	int MaxArraySize = 0;
	
	std::vector <std::string> varIntList;
	std::vector <std::string> varTabIntList;
	
	std::vector <OutsideFunction> functions;
	
public:	
	//main
	McFunction();
	McFunction(const McFunction &mcfunction);
	void operator=(const McFunction &mcfunction);
	std::string getName();
	
	
	
	void setOutsideFunctions (std::vector <OutsideFunction> functions);
	
	
	void Rename(std::string name);
	void newContent();
	void addContent(std::string content);
	void clean();
	void RenameAndNewContent(std::string name);
	void CopyAndRename(std::string name, McFunction dad);
	void setVarsLists(std::vector <std::string> IntList, std::vector <std::string> TabIntList);
	
	int parseStatement(Statement comm);
	void SaveFunction(std::string dir);
	
	
protected:
	//run
	std::string McRun(std::string location, std::string a);
	
	
	
	//int
	std::string McAddInt(std::string a);
	std::string McAddIntTab(std::string a);
	
	//table
	std::string GetArrayName(std::string a);
	std::string GetArrayIndex(std::string a);
	
	
	std::string McCreateNewIntArray(std::string a);
	std::string McGetValueFromIntArray(std::string array, std::string index);
	std::string McSetValueOnIntArray(std::string array, std::string index, std::string value);
	std::string McPushValueToIntArray(std::string array, std::string value);
	std::string McExpandIntArrayByOne(std::string array);
	
	//keywords
	std::string parseSay(Statement stat);
	std::string parseIf(Statement stat);
	std::string parseScoreCondition(Statement stat);
	std::string parseModifySubcommand(Statement stat);
	
	//operation
	std::string McOperationInt(Statement comm);
	//=
	std::string McIntSet(std::string a, std::string b);
	
	//+ -
	std::string McIntAdd(std::string result, std::string a, std::string b);
	std::string McIntAddEqual(std::string result, std::string a);
	std::string McIntSubtract(std::string result, std::string a, std::string b);
	std::string McIntSubtractEqual(std::string result, std::string a);
	//* =
	std::string McIntMultiply(std::string result, std::string a, std::string b);
	std::string McIntMultiplyEqual(std::string result, std::string a);
	std::string McIntDivide(std::string result, std::string a, std::string b);
	std::string McIntDivideEqual(std::string result, std::string a);
	//%
	std::string McIntModulo(std::string result, std::string a, std::string b);
	std::string McIntModuloEqual(std::string result, std::string a);
	//><
	std::string McIntSwap(std::string a, std::string b);
	
	bool isNumber(std::string word);
	bool isCmpOperator(std::string word);
	bool IfArray(std::string a);
	void Clear();

};

#endif
