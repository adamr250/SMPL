#include "Lekser.h"

void Lekser::processRun(){
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