#include "Lekser.h"

//main method, invokes other methods to obtain a vector of Statements out of a string or a file
void Lekser::lekser(std::string source, int type){
    cleanUp();
    //std::string inputToProcess = readInput(source, type);
    std::vector<std::vector<char>> input = readInput2(source, type);
    //processInput(inputToProcess);
    int processingResult = proccessAndValidate(input);//std::cout << "RESULT : " << processingResult << std::endl; //TODO rem
    if (processingResult == 0){
        //std::cout << "SUCCESS" << std::endl; //TODO rem
        //*
        processInit();
        processRun();
        //*/
    }
}