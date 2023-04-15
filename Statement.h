#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include <string>
#include <vector>

class Statement {
public:
    std::string name = "";
    int argc = 0;
    std::vector <std::string> argv;
    std::vector <Statement> statv;

    void wypiszArgv() { //wypisuje vector argv
        for(int i=0; i<(int)argv.size(); i++) {
            std::cout << argv[i];
            if(i < ((int)argv.size()-1))
                std::cout << ", ";
        }
        std::cout << std::endl;
    }

    void wypiszStatv() {
        for(Statement S : statv) {
            std::cout << "\ts_name: " << S.name << "; s_argc: " << S.argc << "; s_argv: ";
            for(std::string s_argv : S.argv){
                std::cout << s_argv << ", ";
            }
            std::cout << std::endl;
        }
    }

    void wypisz() { //wypisuje zawartosc klasy (name, argc, argv, statv)
        std::cout << "name: " << name << "; argc: " << argc << "; argv: ";
        wypiszArgv();
        wypiszStatv();
    }

    void clear() {
        name="";
        argc=0;
        argv.clear();
        statv.clear();
    }
};





#endif
