//FOR TESTS ONLY

#include "Lekser.h"
#include <iostream>
#include <fstream>

//----------pobieranie wartości z klasy Statement------------------
std::vector<Statement> Lekser::getStatementVector(){
    return statementResult;
}

std::string Lekser::getName() {
    return statementResult[0].name;
}

int Lekser::getArgc() {
    return statementResult[0].argc;
}

std::vector<std::string> Lekser::getArgv() {
    return statementResult[0].argv;
}

Statement Lekser::getNext() {
    Statement StatementNext;
    if(statementResult.empty()) {
        StatementNext.name = "EOF";
        StatementNext.argc = 0;

        return StatementNext;
    }

    StatementNext.name  = getName();
    StatementNext.argc  = getArgc();
    StatementNext.argv  = getArgv();
    StatementNext.statv = statementResult[0].statv;

    statementResult.erase(statementResult.begin());
    return StatementNext;
}
//------------------------------------------------------------


int main(){

    Lekser k;
    //std::cout << "TEST 1: " << std::endl;
    k.lekser("example.txt", 1);
    //k.displayInput();
    //std::cout << "TEST 2: " << std::endl;
    //k.lekser("<init>melanoma zaboj;    cza czlo;wieka wieka;<run>x = x + y;", 2);
    std::cout << std::endl;
    //k.displayInput();
    Statement test;

    while(test.name != "EOF") {
        test = k.getNext();
        test.wypisz();
    }
    //std::cout << k.getStatementVector()[0].name << std::endl;
    //*
    /*std::cout << "TEST createArgv:" << std::endl;
    std::vector<std::string> output = k.createArgv("abc,, \"def, ghi,,\",, mno,prs", ",,");
    for (unsigned int i = 0; i < output.size(); i++){
        std::cout << output[i] << std::endl;
    }//*/

    return 0;
}
