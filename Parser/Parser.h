#include <stack>
#include <string>
#include <vector>
#include "../Statement.h"
#include "McFunction.h"
#include "../Names.h"

#define IDLE -1
#define INIT 0
#define RUN 1
#define FUNCTION 2

class Parser { 
protected:
	std::string Name = "";
	std::string OutPath = ".";
	
	int state = IDLE;
	McFunction mcfunction;
	std::stack<McFunction> functions;
	std::vector<std::string> functions_names;
	std::stack<int> function_numbers;

public:
	void setName(std::string name);
	void setOutPath(std::string outPath);
	int parseStatement(Statement stat);

	void loadOutsideFunctions(std::string path);
	OutsideFunction loadOutsideFunction(std::string filePath);
	void setOutsideFunctions(std::vector <OutsideFunction> functions);
};
