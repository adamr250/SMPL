#ifndef OUTSIDE_FUNCTIONS_H
#define OUTSIDE_FUNCTIONS_H


#include <iostream>
#include <string>
#include <vector>
#include "../Statement.h"

class Task {
public:
	int type = 0;
	// 0 - text
	// 1 - arg scoreboard player
	// 2 - arg scoreboard name or int 
	// 3 - arg entiti selector
	// 4 - arg nbtdata
	// 5 - arg string
	// 6 - endl
	// 7 - coordinates elemet
	int minArgRequirement = 0;
	std::string content = "";
	int extra = 0;
	
	
	Task() {
		this->type = 0;
		this->minArgRequirement = 0;
		this->content = "";
		this->extra = 0;
	}
	Task(int type, int minArgRequirement, std::string content, int extra) {
		this->type = type;
		this->minArgRequirement = minArgRequirement;
		this->content = content;
		this->extra = extra;
	}
	void print() {
		std::cout << this->type << ' ' << this->minArgRequirement << " \"" << this->content << "\" " << this->extra << '\n';
	}
};


class OutsideFunction {
public:
	std::string Name = "";
	int InputMniArgs = 0;
	int InputArgs = 0;
	std::vector <int> InputArgsTypes;
	// 1 - arg scoreboard var
	// 3 - arg entiti selector
	// 4 - arg nbtdata
	// 5 - arg string
	// 7 - coordinates_elemet
	
protected:
	std::vector <Task> Tasks;
public:
	OutsideFunction();
	OutsideFunction(std::string name, int inputArgs, int InputMniArgs, std::vector <int> inputArgsTypes);
	void operator=(const OutsideFunction &outsideFunction);
	void AddJob(Task task);
	void SetJobs(std::vector <Task> tasks);
	std::string parseStatement(Statement stat) ;
	
};

#endif
	
