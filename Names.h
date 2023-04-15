#ifndef NAMES_H
#define NAMES_H

#include <string>

class Names {
private:
	static int functionsCount;

public:
    static std::string generateFunctionName();
};

#endif
