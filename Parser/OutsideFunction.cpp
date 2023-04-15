#include "OutsideFunction.h"



OutsideFunction::OutsideFunction() {
	this->Name = "";
	this->InputArgs = 0;
	this->InputMniArgs = 0;
}
OutsideFunction::OutsideFunction(std::string name, int inputArgs, int InputMniArgs, std::vector <int> inputArgsTypes) {
	this->Name = name;
	this->InputArgs = inputArgs;
	this->InputMniArgs = InputMniArgs;
	this->InputArgsTypes = inputArgsTypes;
}
void OutsideFunction::operator=(const OutsideFunction &outsideFunction) {
	this->Name = outsideFunction.Name;
	this->InputMniArgs = outsideFunction.InputMniArgs;
	this->InputArgs = outsideFunction.InputArgs;
	this->InputArgsTypes = outsideFunction.InputArgsTypes;
	this->Tasks = outsideFunction.Tasks;
}
void OutsideFunction::AddJob(Task task) {
	this->Tasks.push_back(task);
}
void OutsideFunction::SetJobs(std::vector <Task> tasks) {
	this->Tasks = tasks;
}

std::string OutsideFunction::parseStatement(Statement stat) {
	std::string Out = "";
	std::string a = "";
	
	for(Task task : this->Tasks) {
		printf("%d %d\n",task.minArgRequirement,stat.argc);
		if(task.minArgRequirement <= stat.argc) {
			task.print();
			switch(task.type) {
				case 0:
					Out += task.content;
					break;
				case 1:
					if(stat.argv[task.extra] == "") {
						Out = "-1";
						break;
					}
					//scoreboard player
					a = stat.argv[task.extra];
					Out += a.substr((int)(a.find(' ')+1));
					break;
				case 2:
					if(stat.argv[task.extra] == "") {
						Out = "-1";
						break;
					}
					//scoreboard name
					a = stat.argv[task.extra];
					Out += a.substr(0,(int)(a.find(' ')));
					break;
				case 3:
					if(stat.argv[task.extra] == "") {
						Out = "-1";
						break;
					}
					//entiti
					if(stat.argv[task.extra][0] == '@') {
						Out += stat.argv[task.extra];
					}else {
						Out = "-1";
					}
					break;
				case 4:
					if(stat.argv[task.extra] == "") {
						Out = "-1";
						break;
					}
					//nbt
					Out += stat.argv[task.extra];
					break;
				case 5:
					//string
					//"" must be remove before it comes
					Out += stat.argv[task.extra];
					break;
				case 6:
					
					Out += "\n";
					break;
				case 7:
					if(stat.argv[task.extra] == "") {
						Out = "-1";
						break;
					}
					if(stat.argv[task.extra][0] == '~' || stat.argv[task.extra][0] == '^') {
						if(stat.argv[task.extra].length() != 1) {
							if(stat.argv[task.extra][1] == '-' || std::isdigit(stat.argv[task.extra][1]) == 1) {
								
								for(int i = 2;i < (int)stat.argv[task.extra].length(); i++) {
									if(std::isdigit(stat.argv[task.extra][i]) == 0) {
										Out = "-1";
										break;
									}
								}
							}
						}
					} else if (stat.argv[task.extra][0] == '-' || std::isdigit(stat.argv[task.extra][0]) == 1) {
						for(int i = 1;i < (int)stat.argv[task.extra].length(); i++) {
							if(std::isdigit(stat.argv[task.extra][i]) == 0) {
								Out = "-1";
								break;
							}
						}
					}else {
						Out = "-1";
						break;
					}
					Out += stat.argv[task.extra];
					break;
				default:
					Out = "-1";
			}
		}
		if(Out == "-1") {
			break;
		}
	}
	return Out;
}
	
