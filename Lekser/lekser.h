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

public:
    //main method, invokes other methods to obtain a vector of Statements out of a string or a file
    void lekser(std::string source, int type){
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

    // short: reads either a file or a string and returns a single string
    std::string readInput(std::string source, int type){
        std::string inputUnprocessed = "";
        switch(type){
            /* file is the source */
            case 1:{
                std::ifstream ifs(source);
                std::string line;
                while (std::getline(ifs, line)){
                    inputUnprocessed += line;
                    inputUnprocessed += " ";
                }
                ifs.close();
                break;
            }
            /* string is the source */
            case 2:{
                std::stringstream ss(source);
                std::string line;
                while(std::getline(ss,line,'\n')){
                    inputUnprocessed += line;
                    inputUnprocessed += " ";
                }
                break;
            }
            default: /* error? */ break;
        }
        return inputUnprocessed;
    }

    std::vector<std::vector<char>> readInput2(std::string source, int type){
        std::vector<std::vector<char>> inputRaw;
        std::vector<std::string> inputUncommented;
        switch(type){
            /* file is the source */
            case 1:{
                std::ifstream ifs(source);
                std::vector<char> line;
                char character = 0;

                int openQuotMarks = 0;
                int ignoreCommentChars = 0;
                int specialCommandChars = 0;
				int openBrackets = 0;
                while (ifs.get(character)){
                    switch(character){
                        case '\n':{
                            if (ignoreCommentChars == 1){
                                ignoreCommentChars = 0;
                            }
                            if (specialCommandChars){
                                specialCommandChars = 0;
                                inputRaw.push_back(line);
                                line.clear();
								openBrackets = 0;
                            }
                            break;
                        }
                        case '#':{
                            if (ignoreCommentChars == 0){
                                if (!line.empty()){
                                    if (line.back() == '\\'){
                                        line.push_back(character);
                                    } else {
                                        ignoreCommentChars = 1;
                                    }
                                } else{
                                    ignoreCommentChars = 1;
                                }
                            }
                            break;
                        }
                        case '$':{
                            if (ignoreCommentChars == 0){
                                if (openQuotMarks){
                                    line.push_back(character);
                                } else {
                                    specialCommandChars = 1;
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
                        case '"':{
                            if (ignoreCommentChars == 0){
                                if (line.back() != '\\'){
                                    openQuotMarks = (openQuotMarks + 1) % 2;
                                }
                                line.push_back(character);
                            }
                            break;
                        }
                        case '>':{
                            if (ignoreCommentChars == 0){
                                int wsCounter = 0;
                                while (line[wsCounter] == ' '){
                                    wsCounter++;
                                }
                                if ((line[wsCounter] == '<' && line[wsCounter + 1] == 'r' && line[wsCounter + 2] == 'u' && line[wsCounter + 3] == 'n')
                                || (line[wsCounter] == '<' && line[wsCounter + 1] == 'i' && line[wsCounter + 2] == 'n' && line[wsCounter + 3] == 'i' && line[wsCounter + 4] == 't')){
                                    line.push_back(character);
                                    inputRaw.push_back(line);
                                    line.clear();
									openBrackets = 0;
                                } else{
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
						case '(':{
                            if (ignoreCommentChars == 0){
                                if ((openQuotMarks == 0) && (specialCommandChars == 0)){
                                    line.push_back(' ');
                                }
                                openBrackets++;
                                line.push_back(character);
                            }
							break;
						}
                        case '[':{
                            if (ignoreCommentChars == 0){
                                openBrackets++;
                                line.push_back(character);
                            }
							break;
						}
						case ')':
                        case ']':{
                            if (ignoreCommentChars == 0){
                                openBrackets--;
                                line.push_back(character);
                            }
							break;
						}
                        case ';':
                        case '}':
                        case '{':{
                            if (ignoreCommentChars == 0){
                                if (openQuotMarks || specialCommandChars){
                                    line.push_back(character);
                                } else if (openBrackets > 0){
									line.push_back(character);
								} else {
                                    line.push_back(character);
                                    inputRaw.push_back(line);
                                    line.clear();
									openBrackets = 0;
                                }
                            }
                            break;
                        }
                        /*case ' ':{
                            if (ignoreCommentChars == 0){
                                if (line.back() != ' '){
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
                        case '\t':{
                            if (ignoreCommentChars == 0){
                                //if (line.back() != ' '){
                                    line.push_back(' ');
                                //}
                            }
                            break;
                        }*/
                        default:{
                            if (ignoreCommentChars == 0){
                                line.push_back(character);
                            }
                            break;
                        }
                    }
                }
                ifs.close();
                break;
            }
            /* string is the source */
            case 2:{
                std::stringstream ss(source);
                std::vector<char> line;
                char character = 0;

                int openQuotMarks = 0;
                int ignoreCommentChars = 0;
                int specialCommandChars = 0;
				int openBrackets = 0;
                while (ss.get(character)){
                    switch(character){
                        case '\n':{
                            if (ignoreCommentChars == 1){
                                ignoreCommentChars = 0;
                            }
                            if (specialCommandChars){
                                specialCommandChars = 0;
                                inputRaw.push_back(line);
                                line.clear();
								openBrackets = 0;
                            }
                            break;
                        }
                        case '#':{
                            if (ignoreCommentChars == 0){
                                if (line.back() == '\\'){
                                    line.push_back(character);
                                } else {
                                    ignoreCommentChars = 1;
                                }
                            }
                            break;
                        }
                        case '$':{
                            if (ignoreCommentChars == 0){
                                if (openQuotMarks){
                                    line.push_back(character);
                                } else {
                                    specialCommandChars = 1;
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
                        case '"':{
                            if (ignoreCommentChars == 0){
                                if (line.back() != '\\'){
                                    openQuotMarks = (openQuotMarks + 1) % 2;
                                }
                                line.push_back(character);
                            }
                            break;
                        }
                        case '>':{
                            if (ignoreCommentChars == 0){
                                if ((line[0] == '<' && line[1] == 'r' && line[2] == 'u' && line[3] == 'n')
                                || (line[0] == '<' && line[1] == 'i' && line[2] == 'n' && line[3] == 'i' && line[4] == 't')){
                                    line.push_back(character);
                                    inputRaw.push_back(line);
                                    line.clear();
									openBrackets = 0;
                                } else{
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
						case '(':{
                            if (ignoreCommentChars == 0){
                                if ((openQuotMarks == 0) && (specialCommandChars == 0)){
                                    line.push_back(' ');
                                }
                                openBrackets++;
                                line.push_back(character);
                            }
							break;
						}
                        case '[':{
                            if (ignoreCommentChars == 0){
                                openBrackets++;
                                line.push_back(character);
                            }
							break;
						}
						case ')':
                        case ']':{
                            if (ignoreCommentChars == 0){
                                openBrackets--;
                                line.push_back(character);
                            }
							break;
						}
                        case ';':
                        case '}':
                        case '{':{
                            if (ignoreCommentChars == 0){
                                if (openQuotMarks || specialCommandChars){
                                    line.push_back(character);
                                } else if (openBrackets > 0){
									line.push_back(character);
								} else {
                                    line.push_back(character);
                                    inputRaw.push_back(line);
                                    line.clear();
									openBrackets = 0;
                                }
                            }
                            break;
                        }
                        /*case ' ':{
                            if (ignoreCommentChars == 0){
                                if (line.back() != ' '){
                                    line.push_back(character);
                                }
                            }
                            break;
                        }
                        case '\t':{
                            if (ignoreCommentChars == 0){
                                //if (line.back() != ' '){
                                    line.push_back(' ');
                                //}
                            }
                            break;
                        }*/
                        default:{
                            if (ignoreCommentChars == 0){
                                line.push_back(character);
                            }
                            break;
                        }
                    }
                    //inputUnprocessed += line;
                    //inputUnprocessed += " ";
                }
                break;
            }
            default: /* error? */ break;
        }
        return inputRaw;
    }

    // puts each "logic line" into input vector.
    // naive splitting - need to check if symbols are not used in a text
    // maybe all special symbols need escaping when used in a non-special manner?
    // after testing - backslash is escaping itself, so either double backslash or a different method
    void processInput(std::string input){
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
    int proccessAndValidate(std::vector<std::vector<char>> inputRaw){
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

    void processInit(){
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


    void processRun(){
        while (logicLinesCurrentPosition < logicLines.size()){
            Statement stat;
            std::string currentLine = logicLines[logicLinesCurrentPosition];
            trim(currentLine);
            if(currentLine == "<run>") {
                stat.name = "<run>";
                statementResult.push_back(stat);
                logicLinesCurrentPosition++;
                continue;
            }
            if(currentLine[0]=='$'){
                currentLine.insert(1," ");
            }
            ///dostawianie spacji przed nawiasem otwierajacym "("
            size_t insertSpace = std::string::npos;
            while((insertSpace=currentLine.find("(",insertSpace+2)) != std::string::npos){
                currentLine.insert(insertSpace, " ");
            }
            std::istringstream iss(currentLine);
            std::string subs, subs_2;
            iss >> subs;
            switch(switchMap[subs]) {
                case 1: { /// obsluga "function"
                    stat.name=subs;
                    while(iss >> subs) {
                        stat.argv.push_back(subs);
                    }
                    stat.argc = stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 2: { /// obsluga funkcji "say"
                    std::vector<std::string> argvVector;
                    stat.name=subs;
                    currentLine.replace(currentLine.find("say"), 3, " ");
                    currentLine.erase(currentLine.end() - 1);
                    argvVector = createArgv(currentLine, "+");
                    for(std::string s : argvVector) {
                        stat.argv.push_back(s);
                    }
                    stat.argc=stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 3: {   ///obsluga "if", "while" i "unless"
                    stat.name=subs;
                    currentLine = currentLine.substr(currentLine.find("(") + 1);
                    currentLine.erase(currentLine.begin()+currentLine.find_last_of(")"), currentLine.end());


                    if(currentLine.find("@") != std::string::npos) {  /// if_entity
                        stat.argv.push_back(currentLine);
                    } else if(currentLine.find(",") != std::string::npos) { /// if_block
                        std::vector<std::string> argvVector = createArgv(currentLine, ",");
                        for(std::string s : argvVector) {
                            stat.argv.push_back(s);
                        }
                    } else { /// if
                        ///szukanie symboli porownania ( ==, !=, <, <=, >, >= ) i wstawianie spacji
                        size_t foundValue=0;
                        for(std::string s : comparisonSymbol){
                            if( (foundValue=currentLine.find(s)) != std::string::npos) {
                                currentLine.insert(foundValue+s.length(), " ");
                                currentLine.insert(foundValue, " ");
                            }
                        }
                        std::istringstream iss2(currentLine);
                        while(iss2 >> subs) {
                            stat.argv.push_back(subs);
                        }
                    }
                    stat.argv.push_back("{");
                    stat.argc=stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 4: { /// zamykajacy nawias klamrowy
                    stat.name=subs;
                    stat.argc = 0;
                    statementResult.push_back(stat);
                    break;
                }
                case 5: {  /// obsluga "run"
                    int loopSize=0;
                    std::string tmpString="";
                    stat.name=subs;
                    iss >> subs;
                    loopSize = subs.length();
                    int i;
                    for(i=0; i<loopSize; i++) {
                        if(subs[i]==':' && subs[i+1]==':') {
                            if(tmpString!="std")
                                stat.argv.push_back(tmpString);
                            tmpString="";
                            break;
                        }
                        tmpString+=subs[i];
                    }
                    for(i+=2; i<loopSize-1; i++) {
                        tmpString+=subs[i];
                    }
                    stat.argv.push_back(tmpString);
                    stat.argc=stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 6: { /// obsluga $
                    stat.name=subs;
                    while(iss >> subs){
                        if(subs[subs.length()-1] == ';'){
                            subs.erase(subs.end() - 1);
                        }
                        stat.argv.push_back(subs);
                    }
                    stat.argc=stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 7: { /// obsluga "for"
                    std::istringstream iss;
                    stat.name = subs;
                    std::vector<std::string> argv;
                    Statement statStatV;

                    currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                    currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
                    currentLine = currentLine.substr(currentLine.find('(') + 1);
                    currentLine.erase(currentLine.end()-2, currentLine.end());

                    while(currentLine.find(';') != std::string::npos) {
                        currentLine.replace(currentLine.find(';'), 1, " ");
                    }

                    iss.clear();
                    iss.str(currentLine);
                    while(iss >> subs){
                        argv.push_back(subs);
                    }
                    ///tworzenie pierwszego statv
                    size_t found = argv[0].find("=");
                    argv[0][found] = ' ';
                    iss.clear();
                    iss.str(argv[0]);
                    while(iss >> subs) {
                        statStatV.argv.push_back(subs);
                    }
                    statStatV.argc = statStatV.argv.size();
                    stat.statv.push_back(statStatV);
                    stat.argc++;
                    statStatV.clear();

                    ///tworzenie drugiego statv
                    for(int i=0; i<6; i++) {
                        found = argv[1].find(comparisonSymbol[i]);
                        if(found != std::string::npos) {
                            currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                            currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
                            argv[1].insert(found+comparisonSymbol[i].length(), " ");
                            argv[1].insert(found, " ");

                            statStatV.name=comparisonSymbol[i];
                            iss.clear();
                            iss.str(argv[1]);
                            while(iss >> subs) {
                                statStatV.argv.push_back(subs);
                            }
                            statStatV.argc = statStatV.argv.size();
                            stat.statv.push_back(statStatV);
                            stat.argc++;
                            statStatV.clear();

                            break;
                        }
                    }

                    ///tworzenie trzeciego statv

                    /// Znadujemy znak rownosci i tworzymy "name" po przez sklejenie go ze znakiem poperzedzajacym,
                    /// ktory powinien byc jedenym z 4 znakow matematycznych (+, -, *, /)
                    found=argv[2].find("=");
                    statStatV.name += argv[2][found-1];
                    statStatV.name += argv[2][found];
                    argv[2].replace(argv[2].find(statStatV.name), 2, " ");
                    iss.clear();
                    iss.str(argv[2]);

                    while(iss >> subs) {
                        statStatV.argv.push_back(subs);
                    }
                    statStatV.argc = statStatV.argv.size();
                    stat.statv.push_back(statStatV);
                    stat.argc++;

                    statementResult.push_back(stat);
                    break;
                }
                case 8: { /// obsluga "at" i "as"
                    currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                    currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
                    stat.name = subs;
                    currentLine = currentLine.substr(currentLine.find("(")+1);
                    currentLine.erase(currentLine.end()-2, currentLine.end());
                    stat.argv.push_back(currentLine);
                    stat.argv.push_back("{");
                    stat.argc = stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                case 9: { ///obsluga "positioned"
                    stat.name = subs;
                    std::string subs;
                    currentLine = currentLine.substr(currentLine.find("(")+1);
                    currentLine.erase(currentLine.begin()+currentLine.find_last_of(")"), currentLine.end());
                    if(currentLine.find("@") != std::string::npos){
                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
                        stat.argv.push_back(currentLine);
                    } else {
                        iss.clear();
                        iss.str(currentLine);
                        while(iss >> subs){
                            stat.argv.push_back(subs);
                        }
                    }
                    stat.argv.push_back("{");
                    stat.argc = stat.argv.size();
                    statementResult.push_back(stat);
                    break;
                }
                default: {
                    ///obsluga array.push
                    if(currentLine.find(".push") != std::string::npos) {
                        stat.name = "push";
                        std::string subs;

                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());

                        currentLine.replace(currentLine.find(".push("), 6, " ");
                        currentLine.erase(currentLine.end()-2, currentLine.end());
                        iss.clear();
                        iss.str(currentLine);
                        while(iss >> subs) {
                            stat.argv.push_back(subs);
                        }
                        stat.argc = stat.argv.size();
                        statementResult.push_back(stat);
                        break;
                    }
                    ///obsluga operacji algebraiczych
                    if(currentLine.find("=") != std::string::npos) {
                        tmpVariableVector.clear();
                        newEquationVector.clear();
                        tmpVariableCount = 0;
                        createTmpVariable();

                        bool isFirstVariableInEquation=0;
                        bool isAssignment = true;
                        int loopSize=0;
                        std::vector<std::string> processedEquationLine;
                        std::string first, extractedBrackets;

                        size_t foundBraces, foundSquareBraces;
                        std::string currentLineReplace="";
                        int foundCount=0;

                        if(isSymbol(std::string(1, currentLine[currentLine.find("=")-1]))) {
                            stat.name = std::string(1, currentLine[currentLine.find("=")-1] ) + "=";
                            currentLine.replace(currentLine.find("=")-1, 2, " ");
                            iss.clear();
                            iss.str(currentLine);
                            while(iss >> subs) {
                                stat.argv.push_back(subs);
                            }
                            stat.argc = stat.argv.size();
                            statementResult.push_back(stat);
                            break;
                        }
                        ///szukanie znakow operacji algebraicznych (+, -, *, /)
                        for(std::string s : mathSymbol){
                            if(currentLine.find(s) != std::string::npos){
                                isAssignment = false;
                            }
                        }

                        ///jezeli nie znalezlismy znakow to znaczy, ze jest to proste przypisanie wartosci
                        if(isAssignment){
                            currentLine.insert(currentLine.find("=")+1," ");
                            currentLine.insert(currentLine.find("=")," ");
                            makeEquationStatement(currentLine);
                            break;
                        }

                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
                        currentLine.erase(remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());

                        ///usuwnaie lewej strony rownania i zapisanie w zmiennej string "first"
                        std::stringstream ss(currentLine);
                        std::getline(ss, first, '=');

                        currentLine = currentLine.substr(currentLine.find("=") + 1);
                        ///sprawdzanie czy zmienna do ktorej zapisujemy wynik wystepuje w samym rownaniu
                        if(currentLine.find(first) != std::string::npos){
                            isFirstVariableInEquation = true;
                        }

                        foundSquareBraces=currentLine.find(']');
                        while(foundSquareBraces != std::string::npos) {
                            foundCount++;
                            foundSquareBraces = currentLine.find(']',foundSquareBraces+1);
                        }

                        while(foundCount--) {
                            extractedBrackets="";
                            foundSquareBraces=0;
                            for(int i=0; i<=foundCount; i++) {
                                 foundSquareBraces=currentLine.find(']', foundSquareBraces+1);
                            }
                            for(int i=foundSquareBraces; i>0; i--){
                                extractedBrackets = currentLine[i] + extractedBrackets;
                                if(currentLine[i]=='['){
                                    break;
                                }
                            }
                            createTmpVariable();stat.argv.push_back(subs);
                            currentLineReplace = "[" + tmpVariableVector.back() + "]";
                            currentLine.replace(currentLine.find(extractedBrackets), extractedBrackets.length(), currentLineReplace);
                            extractedBrackets.erase(0, 1);
                            extractedBrackets.erase(extractedBrackets.end()-1);
                            processedEquationLine.push_back(tmpVariableVector.back() + " = " + extractedBrackets);
                        }
                        if(!isFirstVariableInEquation){
                            tmpVariableVector[0]=first;
                        }
                        processedEquationLine.push_back(tmpVariableVector[0] + " = " + currentLine);

                        loopSize = processedEquationLine.size();
                        for(int j=loopSize-1; j>=0; j--) {
                            foundBraces=processedEquationLine[j].find(')');
                            while(foundBraces != std::string::npos) {
                                extractedBrackets="";
                                for(int i=processedEquationLine[j].find(')'); i>=0; i--){
                                    extractedBrackets = processedEquationLine[j][i] + extractedBrackets;
                                    if(processedEquationLine[j][i]=='(')
                                        break;
                                }
                                createTmpVariable();
                                processedEquationLine[j].replace(processedEquationLine[j].find(extractedBrackets), extractedBrackets.length(), tmpVariableVector.back());
                                extractedBrackets.erase(0, 1);
                                extractedBrackets.erase(extractedBrackets.end()-1);
                                processedEquationLine.insert(processedEquationLine.begin()+(j++), tmpVariableVector.back() + " = " + extractedBrackets);
                                foundBraces=processedEquationLine[j].find(')');
                            }
                        }

                        loopSize = processedEquationLine.size();
                        int loopSize_2;
                        for(int i=0; i<loopSize; i++) {

                            loopSize_2 = processedEquationLine[i].length();
                            for(int j=0; j<loopSize_2; j++) {
                                if(processedEquationLine[i][j]== '+' || processedEquationLine[i][j]=='-'){
                                    processedEquationLine[i].insert(j," ");
                                    processedEquationLine[i].insert(j+2," ");
                                    j++;
                                    loopSize_2+=2;
                                }
                            }
                            size_t foundMultiply, foundDiv;
                            std::stringstream ss2(processedEquationLine[i]);
                            if(processedEquationLine[i].find("+")!=std::string::npos || processedEquationLine[i].find("-")!=std::string::npos) {
                                while(ss2 >> subs) {
                                    foundMultiply = subs.find('*');
                                    foundDiv      = subs.find('/');
                                    if((foundMultiply != std::string::npos) || (foundDiv != std::string::npos)){
                                        std::string tmpSubs = subs;
                                        createTmpVariable();
                                        loopSize_2=tmpSubs.length();
                                        for(int k=0; k<loopSize_2; k++) {
                                            if(tmpSubs[k]== '*' || tmpSubs[k]=='/'){
                                                tmpSubs.insert(k," ");
                                                tmpSubs.insert(k+2," ");
                                                k++;
                                                loopSize_2+=2;
                                            }
                                        }
                                        newEquationVector.push_back(tmpVariableVector.back() + " = " + tmpSubs);
                                        processedEquationLine[i].replace(processedEquationLine[i].find(subs), subs.length(), tmpVariableVector.back());
                                    }
                                }
                            } else {
                                loopSize_2 = processedEquationLine[i].length();
                                for(int j=0; j<loopSize_2; j++) {
                                    if(processedEquationLine[i][j]== '*' || processedEquationLine[i][j]=='/'){
                                        processedEquationLine[i].insert(j," ");
                                        processedEquationLine[i].insert(j+2," ");
                                        j++;
                                        loopSize_2+=2;
                                    }
                                }
                            }
                            newEquationVector.push_back(processedEquationLine[i]); ///rownanie z podmienionymi mnozeniem/dzieleniem na zmienna tymczasowa ( x*y -> <tmpVarX> )
                        }
                        for(std::string S : newEquationVector) {
                            makeEquationStatement(S);
                        }
                        if(isFirstVariableInEquation){
                            makeEquationStatement(first + " = " + tmpVariableVector[0]);
                        }
                    } else {
                        ///otherFuncion
                        std::vector<std::string> argvVector;
                        stat.name = subs;
                        currentLine = currentLine.substr(currentLine.find("(")+1);
                        currentLine.erase(currentLine.begin()+currentLine.find_last_of(")"), currentLine.end());
                        argvVector = createArgv(currentLine, ",");
                        for(std::string s : argvVector){
                            stat.argv.push_back(s);
                        }
                        stat.argc = stat.argv.size();
                        statementResult.push_back(stat);
                    }
                    break;
                }
            }
            logicLinesCurrentPosition++;
        }
        argvEntitiSelectorConnect();
        argvNbtDataConnect();
    }

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

    void cleanUp(){
        logicLines.erase(logicLines.begin(), logicLines.end());
        logicLinesCurrentPosition = 0;
    }

    //TEST only
    void displayInput(){
        for (unsigned int i = 0; i < logicLines.size(); i++){
            std::cout << logicLines[i] << std::endl;
        }
    }
    //TEST only
    std::vector<Statement> getStatementVector(){
        return statementResult;
    }

    std::string getName() {
        return statementResult[0].name;
    }

    int getArgc() {
        return statementResult[0].argc;
    }

    std::vector<std::string> getArgv() {
        return statementResult[0].argv;
    }

    std::vector<std::string> createArgv(std::string input, std::string delimiter){
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

    /*Statement getStatv() {
        return statementResult[0].statv;
    }*/

    Statement getNext() {
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

    bool isSymbol(std::string s) { ///sprawdza czy jest znakiem operacji: *, -, /, +
        if((s=="*") || (s=="/") || (s=="+") || (s=="-")) {
            return true;
        } else {
            return false;
        }
    }

    bool isComparisonSymbol(char c) { ///sprawdza czy jest znakiem wykorzystywanym przy porownywaniu: !, =,  <, >
        if((c=='!') || (c=='=') || (c=='<') || (c=='>')) {
            return true;
        } else {
            return false;
        }
    }

    void createTmpVariable() {
        tmpVariableVector.push_back("<tmpVar" + std::to_string(tmpVariableCount) + ">");
        tmpVariableCount++;
    }

    void makeEquationStatement(std::string line){
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

    void argvEntitiSelectorConnect() {
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

    void argvNbtDataConnect() {
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

    std::map<std::string, int> switchMap {{"function",1}, {"say",2}, {"if",3}, {"unless",3}, {"while",3}, {"}",4}, {"run",5}, {"$",6}, {"for",7}, {"as",8}, {"at",8}, {"positioned",9} };
};
