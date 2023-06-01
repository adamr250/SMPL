#include "Lekser.h"

void Lekser::cleanUp(){
    logicLines.erase(logicLines.begin(), logicLines.end());
    logicLinesCurrentPosition = 0;
}

std::vector<std::string> Lekser::createArgv(std::string input, std::string delimiter){
    std::vector<std::string> result;
    std::string splitString = "";
    unsigned int delimiterCheck = 0;
    int openQuotMarks = 0;
    for (unsigned int i = 0; i < input.size(); i++){
        if (input[i] == '"'){
            openQuotMarks = (openQuotMarks + 1) % 2;
        }
        if (!openQuotMarks && input[i] == ' ') {
            continue;
        }
        splitString += input[i];
        if (!openQuotMarks){
            if (input[i] == delimiter[delimiterCheck]){
                delimiterCheck++;
            } else if (input[i] == delimiter[0]){
                delimiterCheck = 1;
            } else{
                delimiterCheck = 0;
            }
            if (delimiterCheck == delimiter.size()){
                std::string splitStringWithoutDelimiter = splitString.substr(0, splitString.size() - delimiter.size());
                trim(splitStringWithoutDelimiter);
                result.push_back(splitStringWithoutDelimiter);
                splitString.clear();
            }
        }
    }
    result.push_back(splitString);
    return result;
}

bool Lekser::isSymbol(std::string s) { ///sprawdza czy jest znakiem operacji: *, -, /, +
    if((s=="*") || (s=="/") || (s=="+") || (s=="-")) {
        return true;
    } else {
        return false;
    }
}

bool Lekser::isComparisonSymbol(char c) { ///sprawdza czy jest znakiem wykorzystywanym przy porownywaniu: !, =,  <, >
    if((c=='!') || (c=='=') || (c=='<') || (c=='>')) {
        return true;
    } else {
        return false;
    }
}

void Lekser::createTmpVariable() {
    tmpVariableVector.push_back("<tmpVar" + std::to_string(tmpVariableCount) + ">");
    tmpVariableCount++;
}

void Lekser::makeEquationStatement(std::string line){
    Statement stat;
    std::istringstream iss(line);
    std::string subs, first;
    iss>>subs;
    first = subs;
    while(iss>>subs){
        if(subs=="="){
            stat.name=subs;
        } else if(isSymbol(subs)) {
            stat.name = subs+"=";
        } else {
            stat.argv.push_back(first);
            if(subs[subs.length()-1]==';'){
                subs.erase(subs.end() - 1);
            }
            stat.argv.push_back(subs);
            stat.argc=stat.argv.size();
            statementResult.push_back(stat);
            stat.argv.clear();
        }
    }
}

void Lekser::argvEntitiSelectorConnect() {
    int loopSize = statementResult.size();
    int internalLoopSize;
    for(int i=0; i<loopSize; i++) {
        if(statementResult[i].name == "say") {
            continue;
        }
        internalLoopSize = statementResult[i].argv.size();
        for(int j=0; j<internalLoopSize; j++) {
            if(statementResult[i].argv[j][0]=='@') {
                if(statementResult[i].argv[j][2] != '[')
                    break;
                while(statementResult[i].argv[j][statementResult[i].argv[j].length()-1] != ']'){
                    statementResult[i].argv[j]+=statementResult[i].argv[j+1];
                    statementResult[i].argv.erase(statementResult[i].argv.begin()+j+1);
                    internalLoopSize--;
                }
            }
        }
        statementResult[i].argc = statementResult[i].argv.size();
    }
}

void Lekser::argvNbtDataConnect() {
    int vecNum;
    int counter_1, counter_2;
    size_t found_1, found_2;
    bool connect=true;

    int loopSize = statementResult.size();
    int internalLoopSize;

    for(int i=0; i<loopSize; i++) {
        if(statementResult[i].name == "say") {
            continue;
        }
        internalLoopSize = statementResult[i].argv.size();
        counter_1=0;
        counter_2=0;
        vecNum=0;
        if(statementResult[i].name == "$") {
            while(vecNum < internalLoopSize) {
                found_1 = statementResult[i].argv[vecNum].find("{");
                found_2 = statementResult[i].argv[vecNum].find("}");
                connect = counter_1;
                while(found_1 != std::string::npos) {
                    counter_1++;
                    found_1 = statementResult[i].argv[vecNum].find("{",found_1+1);
                }
                while(found_2 != std::string::npos) {
                    counter_2++;
                    found_2 = statementResult[i].argv[vecNum].find("}",found_2+1);
                }
                if(connect) {
                    statementResult[i].argv[vecNum-1]+=statementResult[i].argv[vecNum];
                    statementResult[i].argv.erase(statementResult[i].argv.begin()+vecNum);
                    internalLoopSize--;
                } else {
                    vecNum++;
                }
                if((counter_1 == counter_2) && (counter_1 != 0)) {
                    counter_1 = 0;
                    counter_2 = 0;
                    connect = false;
                }
            }
        }
        statementResult[i].argc = statementResult[i].argv.size();
    }
}