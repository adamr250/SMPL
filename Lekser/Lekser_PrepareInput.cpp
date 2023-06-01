#include "Lekser.h"

// puts each "logic line" into input vector.
// naive splitting - need to check if symbols are not used in a text
// maybe all special symbols need escaping when used in a non-special manner?
// after testing - backslash is escaping itself, so either double backslash or a different method
void Lekser::processInput(std::string input){
    std::string part = "";
    for (unsigned int i = 0; i < input.length(); i++){
        part += input[i];
        for (int j = 0; j < DELIMS_NUM; j++){
            if (input[i] == delims[j]){
                logicLines.push_back(part);
                part = "";
            }
        }
    }
}

// return values:
// 0 - OK
// 1 - not started with <init>
// 2 - init does not have proper syntax (only type and name of a variable/function permitted
// 3 - names of functions don't match (can't do the same with variables as they may come from different sources)
// 4 - function _name_ doesn't end with open brace
// 5 - wrong number of opening and closing braces
// 6 - closing brace without opening brace
int Lekser::proccessAndValidate(std::vector<std::vector<char>> inputRaw){
    int flagInitOver = 0;
    int isItFunctionName = 0;
    int braceCount = 0;
    std::vector<std::string> functionNames;
    for (unsigned int i = 0; i < inputRaw.size(); i++){
        std::string str(inputRaw[i].begin(), inputRaw[i].end());
        ltrim(str);
        //VALIDATE
        if (i == 0){
            if (str.compare("<init>") != 0){
                return 1;
            }
        } else if (str.compare("<run>") == 0){
            flagInitOver = 1;
        } else if (!flagInitOver){
            isItFunctionName = 0;
            std::string temp = "";
            unsigned int j = 0;
            int wordCount = 0;
            while (j < str.size()){
                switch(str[j]){
                    case ' ':{
                        if (!temp.empty()){
                            if (temp.compare("function") == 0){
                                isItFunctionName = 1;
                            }
                            temp.clear();
                            wordCount++;
                        }
                        break;
                    }
                    default:{
                        temp += str[j];
                        if (j == str.size() - 1){
                            if (isItFunctionName){
                                functionNames.push_back(temp.substr(0, temp.size() - 1));
                            }
                            temp.clear();
                            wordCount++;
                        }
                        break;
                    }

                }
                j++;
            }
            if (wordCount > 2){
                return 2;
            }
        } else if (str.substr(0, 8).compare("function") == 0){
            if (braceCount != 0){
                return 5;
            }
            braceCount = 0;
            int doesFunctionNameExist = 0;
            std::string tempFunctionName = str.substr(9, str.size() - 10);
            trim(tempFunctionName);
            for (unsigned j = 0; j < functionNames.size(); j++){
                if (functionNames[j].compare(tempFunctionName) == 0){
                    doesFunctionNameExist = 1;
                }
            }
            if (!doesFunctionNameExist){
                return 3;
            }
            if (str.back() != '{'){
                return 4;
            } else{
                braceCount++;
            }
        } else if (str.substr(0, 1).compare("$") != 0){
            if (str.back() == '{'){
                braceCount++;
            } else if (str.back() == '}'){
                braceCount--;
                if (braceCount < 0){
                    return 6;
                }
            }
            /*int openQuotMarks = 0;
            for (unsigned int j = 0; j < str.size(); j++){
                switch (str[j]){
                    case '"':{
                        if (j > 0){
                            if (str[j - 1] != '\\'){
                                openQuotMarks = (openQuotMarks + 1) % 2;
                            }
                        }
                        break;
                    }
                    case '{':{
                        if (!openQuotMarks){
                            bracesCount[0]++;
                        }
                        break;
                    }
                    case '}':{
                        if (!openQuotMarks){
                            bracesCount[1]++;
                        }
                        break;
                    }
                    default:{
                        break;
                    }
                }
            }*/
        }

        logicLines.push_back(str);
    }
    if (braceCount != 0){
        return 5;
    }
    return 0;
}

void Lekser::processInit(){
    int endProcessing = 0;
    while (!endProcessing){
        Statement statement;
        trim(logicLines[logicLinesCurrentPosition]);
        std::istringstream iss(logicLines[logicLinesCurrentPosition]);
        std::string subs;
        iss >> subs;
        statement.name = subs;
        statement.argc = 1;//logicLines[logicLinesCurrentPosition] != "<run>"
        if (subs == "<init>"){
            statement.argv = {""};
        } else if (subs == "<run>"){
            break;
        } else{
            iss >> subs;
            if (subs.length() > 0) subs.erase(subs.end() - 1);
            statement.argv = {subs};
        }
        statementResult.push_back(statement);
        logicLinesCurrentPosition++;
    }
}