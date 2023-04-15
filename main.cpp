#include "Lekser/lekser.h" 
#include "Parser/Parser.h"
#include <cstring>

int main(int argc, char **argv) {

	bool debug = false;

	if(argc == 1) {
		std::cout << "Nie podano pliku" << std::endl;
		return -1;
	}

	Parser parser;
	parser.setOutPath(".");
	parser.setName("smpl_out");
	for (int i = 1;i < argc; i+=1) {
		std::string tmp = argv[i];
		if(tmp =="-h" || tmp == "--help") {
			std::cout << "-o --output <path>  set output path" << std::endl;
			std::cout << "-n --name <name>    set main mcfunction name " << std::endl;
			std::cout << "-d --debug          show debug info" << std::endl;
			return 0;
		}else if(tmp =="-o" || tmp == "--output") {
			i+=1;
			parser.setOutPath(argv[i]);
		}else if(tmp == "-n" || tmp == "--name") {
			i+=1;
			parser.setName(argv[i]);
		}else if (tmp == "-d" || tmp == "--debug") {
			debug = true;
		}
	}


	std::ifstream inputFile;
	inputFile.open(argv[argc-1]);

	if ( !inputFile.is_open() ) {
		std::cout << "Nie poprawny plik" << std::endl;
		inputFile.close();
		return -1;

	}
	inputFile.close();


	parser.loadOutsideFunctions(argv[0]);
	Statement comm;

	Lekser leks;
	leks.lekser(argv[argc-1], 1);

	while(comm.name != "EOF") {
		comm = leks.getNext();
		comm.wypisz();
		if(parser.parseStatement(comm) != 0) {
			std::cout << "Error In" << std::endl;
			if(debug) comm.wypisz();
		}
	}


	return 0;
}
