#include "Parser.h"
#include <fstream>
#include <filesystem>
#define MAX_LEN_TEXT 2048


void Parser::setName(std::string name) {
	this->Name = name;
}
void Parser::setOutPath(std::string outPath) {
	this->OutPath = outPath;
}
	
void Parser::setOutsideFunctions(std::vector <OutsideFunction> functions) {
	mcfunction.setOutsideFunctions(functions);
}
	
int Parser::parseStatement(Statement stat) {
	if(stat.argc != (int)stat.argv.size()) return -1;
	
	//std::cout<<"stack size:"<<functions.size()<<std::endl;
	
	if(stat.name == "<init>") {
		mcfunction.RenameAndNewContent("init");
		state = INIT;
		std::filesystem::create_directories(this->OutPath + "/smpl/data/" + this->Name + "/functions");
		std::ofstream pack;
		pack.open(this->OutPath + "/smpl/pack.mcmeta", std::ios::out);
			pack << "{\n\"pack\":{\n\"pack_format\": 9,\n\"description\": \"SMPL\"}}";
		pack.close();
		return 0;
	}
	else if(stat.name == "<run>") {
		state = RUN;
		return 0;
	}
		
	if(state == INIT) {
		if(stat.name == "int") {
			return mcfunction.parseStatement(stat);
		}
		else if(stat.name == "function") {
			if(stat.argc == 1) {
				functions_names.push_back(stat.argv[0]);
			}
			else return -1;
			return 0;
		}
	}
	else if(state == RUN) {
		if(stat.name == "EOF") {
			if(functions.size() != 0) return -1;
			mcfunction.SaveFunction(this->OutPath + "/smpl/data/" + Name + "/functions/");
			
			mcfunction.Rename("clear");
			mcfunction.newContent();
			mcfunction.clean();
			mcfunction.SaveFunction(this->OutPath + "/smpl/data/" + Name + "/functions/");
			
			
			return 0;
		}else if(stat.name == "function") {
			if(stat.argc == 2) {
				//std::cout<<"push:"<<mcfunction.getName()<<std::endl;
				functions.push(mcfunction);
				function_numbers.push(0);
				mcfunction.inputStat = stat;
				mcfunction.RenameAndNewContent(stat.argv[0]);
			}
			else return -1;
			return 0;
		}else if(stat.name == "if" || stat.name == "unless" || stat.name == "while" || stat.name == "for" || stat.name == "as" || stat.name == "at" || stat.name == "positioned") {			
			function_numbers.top()++;
			
			std::string function_name;
			if(function_numbers.size()==1) function_name = "internal_function_" + mcfunction.getName() + std::to_string(function_numbers.top());
			else function_name = mcfunction.getName() + "." + std::to_string(function_numbers.top());
			
			if(stat.name == "for"){
				stat.statv[1].argv.push_back(Name + ":");
				stat.statv[1].argv.push_back(function_name);
				stat.statv[1].argc += 2;
			}
			else {
				if(stat.argc == 0) {
					stat.argv.push_back(Name + ":");
					stat.argc++;
				}
				else stat.argv[stat.argc-1] = Name + ":";
				stat.argv.push_back(function_name);
				stat.argc++;
			}
			mcfunction.parseStatement(stat);
			//mcfunction.SaveFunction("./smpl/data/" + Name + "/functions/");
			
			function_numbers.push(0);
			
			functions.push(mcfunction);
			mcfunction.RenameAndNewContent(function_name);
			mcfunction.inputStat = stat;
			
			//std::cout<<"open:"<<std::endl;
			//mcfunction.inputStat.wypisz();
			//std::cout<<"push:"<<mcfunction.getName()<<std::endl;
			
			return 0;
		}else if(stat.name == "}") {
			if(stat.argc == 0) {
				if(functions.size() == 0) return -1;
				mcfunction.SaveFunction(this->OutPath + "/smpl/data/" + Name + "/functions/");
				//std::cout<<"close:"<<std::endl;
				//mcfunction.inputStat.wypisz();
				mcfunction = functions.top();
				//std::cout<<"pop:"<<functions.top().getName()<<std::endl;
				functions.pop();
				function_numbers.pop();
			} else return -1;
		}else if(stat.name == "run") {
			if(stat.argc == 1) {
				stat.argv.push_back(stat.argv[0]);
				stat.argv[0] = Name + ":";
				stat.argc++;
			}
			return mcfunction.parseStatement(stat);
			return 0;
		}
		else {
			return mcfunction.parseStatement(stat);
		}
	}
	
	return 0;
}
void Parser::loadOutsideFunctions(std::string path) {
	std::vector <OutsideFunction> functions;
	OutsideFunction outsideFunction;
	for(int i = path.length()-1; i>=0;i--) {
		if(path[i] == '/') {
			break;
		}else {
			path.erase(i);
		}
	}
	std::cout << path << std::endl;
	path += "lib";
	
	for (const auto & entry : std::filesystem::directory_iterator(path)) {
		std::string tmp = entry.path().string();
		if( tmp[tmp.length()-1] == 'l' &&
			tmp[tmp.length()-2] == 'p' &&
			tmp[tmp.length()-3] == 'm' &&
			tmp[tmp.length()-4] == 's' &&
			tmp[tmp.length()-5] == '.') {
			outsideFunction = loadOutsideFunction(tmp);
			if(outsideFunction.InputArgs == -1) {
				std::cout << "Error In " << tmp << std::endl;
				std::cout << outsideFunction.Name << std::endl;
			}
			functions.push_back(outsideFunction);
		}
	}
	setOutsideFunctions(functions);
}

OutsideFunction Parser::loadOutsideFunction(std::string filePath)  {
	OutsideFunction Out;
	OutsideFunction error;
	error.InputArgs = -1;
	
	
	std::ifstream inputFile;
	
	inputFile.open(filePath);
	
	
	int state = 0;
	// 0 or else - not defined
	// 1 - name
	// 2 - args
	// 3 - tasks
	int minArgRequirement = 0;
	if ( inputFile.is_open() ) {
		while (!inputFile.eof()) {
			std::string tmp;
			inputFile >> tmp;
			if(tmp == "") {
				break;
			}else if(tmp == "<Name>") {
				state = 1;
				if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
			}else if(tmp == "<Arg>") {
				state = 2;
				if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
			}else if(tmp == "<Run>") {
				state = 3;
				if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
			}else {
				if(state == 1) {
					Out.Name = tmp;
				}else if (state == 2) {
					if(tmp == "int") {
						Out.InputArgs += 1;
						Out.InputArgsTypes.push_back(1);
					}else if(tmp == "entiti") {
						Out.InputArgs += 1;
						Out.InputArgsTypes.push_back(3);
					}else if(tmp == "nbtdata") {
						Out.InputArgs += 1;
						Out.InputArgsTypes.push_back(4);
					}else if(tmp == "string") {
						Out.InputArgs += 1;
						Out.InputArgsTypes.push_back(5);
					}else if(tmp == "coordinates_elemet") {
						Out.InputArgs += 1;
						Out.InputArgsTypes.push_back(7);
					}else {
						error.Name = "unknow type";return error;
					}
				}else if (state == 3) {
					Task task;
					task.minArgRequirement = minArgRequirement;
					if(tmp == "<text>") {
						task.type = 0;
						tmp = "";
						while (true) {
							if(inputFile.eof()) {error.Name = "<text> not closed";return error;}
							getline(inputFile,tmp);
							if(tmp == "</text>") break;
							task.content += tmp; 
						}
						task.extra = 0;
						//task.print();
						Out.AddJob(task);
					}else if(tmp == "endl") {
						task.type = 6;
						task.content = "";
						task.extra = 0;
						//task.print();
						Out.AddJob(task);
					}else if(tmp == "arg") {
						if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
						int argNumber;
						inputFile >> argNumber;
						//idk why it work but it works
						if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
						//
						if(argNumber >= Out.InputArgs) {error.Name = "arg dont exist";return error;}
						if(Out.InputArgsTypes[argNumber] == 1) {
							if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
							inputFile >> tmp;
							if(tmp == "player") {
								task.type = 1;
								task.content = "";
								task.extra = argNumber;
							}else if(tmp == "scoreboard") {
								task.type = 2;
								task.content = "";
								task.extra = argNumber;
							} else {
								error.Name = "unknow type";return error;
							}
						}else {
							task.type = Out.InputArgsTypes[argNumber];
							task.content = "";
							task.extra = argNumber;
						}
						//task.print();
						Out.AddJob(task);
					}else if(tmp == "requires_args") {
						if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
						int minArgNumber;
						inputFile >> minArgNumber;
						if(inputFile.eof()) {error.Name = "not correctly completed file";return error;}
						minArgRequirement = minArgNumber;
					}else {
						error.Name = "wrong run command";return error;
					}
					
					
				}else {
					
				}
			}
		
		
		}
		
	} else {
		error.Name = "Couldn't open file + filePath";return error;
	}
	inputFile.close();
	return Out;
}
