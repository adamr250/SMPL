#include "Names.h"
int Names::functionsCount = 0;

std::string Names::generateFunctionName() {
	Names::functionsCount++;
	return "other_function" + std::to_string(functionsCount);
}
