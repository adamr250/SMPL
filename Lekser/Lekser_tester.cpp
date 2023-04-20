//FOR TESTS ONLY

#include "lekser.h"
#include <iostream>
#include <fstream>

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
