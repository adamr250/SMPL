#include "Lekser.h"

// short: reads either a file or a string and returns a single string
std::string Lekser::readInput(std::string source, int type){
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

std::vector<std::vector<char>> Lekser::readInput2(std::string source, int type){
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