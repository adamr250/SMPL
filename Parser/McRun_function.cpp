#include "McFunction.h"
std::string McFunction::McRun(std::string location, std::string a) {
	std::string Out = "";
		Out += "function " + location + a +"\n";
	return Out;
}
