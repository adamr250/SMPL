#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <map>
#include "../Statement.h"

#define DELIMS_NUM 4

class Lekser{

protected:
    char delims[DELIMS_NUM] = {'{', '}', ';', '>'};
    // input generated from a text (either a file or a string), divided into "logic lines"
    // logic line ends with "{", ";" or "}" or is either <init> or <run>
    std::vector<std::string> logicLines;
    std::vector<Statement> statementResult;
    unsigned int logicLinesCurrentPosition;

    std::vector<std::string> tmpVariableVector;
    std::vector<std::string> newEquationVector;
    unsigned int tmpVariableCount = 0;
    std::string comparisonSymbol[6] = {"!=","==","<=","=>","<",">"};
    std::string mathSymbol[4] = {"+","-","*","/"};

    std::map<std::string, int> switchMap {{"function",1}, {"say",2}, {"if",3}, {"unless",3}, {"while",3}, {"}",4}, {"run",5}, {"$",6}, {"for",7}, {"as",8}, {"at",8}, {"positioned",9} };

public:
    std::string readInput(std::string source, int type);
    std::vector<std::vector<char>> readInput2(std::string source, int type);

    void lekser(std::string source, int type);

    void processInput(std::string input);
    int proccessAndValidate(std::vector<std::vector<char>> inputRaw);
    void processInit();


    void processRun();

    void cleanUp();
    void displayInput();
    std::vector<Statement> getStatementVector();
    std::string getName();
    int getArgc();
    std::vector<std::string> getArgv();
    Statement getNext();


    std::vector<std::string> createArgv(std::string input, std::string delimiter);
    bool isSymbol(std::string s);
    bool isComparisonSymbol(char c);
    void createTmpVariable();
    void makeEquationStatement(std::string line);
    void argvEntitiSelectorConnect();
    void argvNbtDataConnect();

    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }
};
