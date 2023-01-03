//
// Created by puki on 2.1.2023.
//

#include "../header/Parser.h"

const std::string &Parser::getWeb() const {
    return web;
}

const std::string &Parser::getPath() const {
    return path;
}

const std::string &Parser::getName() const {
    return name;
}

int Parser::getPriority() const {
    return priority;
}

const std::string &Parser::getTime() const {
    return time;
}

void Parser::setter(std::vector<std::string> &v, std::string comand) {
    size_t pos;
    while((pos = comand.find(" ")) != std::string::npos){
        v.push_back(comand.substr(0,pos));
        comand.erase(0,pos + 1);
    }
    v.push_back(comand);

}
void Parser::timeParser(std::vector<std::string> &v, std::string comand) {
    size_t pos;
    int i = 0;
    //i=0//rok i=1//mesiac i=2//deni=3 meni sa deliemeter //hodina //minuta //sec
    while((pos = comand.find(" ")) != std::string::npos){
        v.push_back(comand.substr(0,pos));
        comand.erase(0,pos + 1);
        i++;
        if(i==3){
            break;
        }
    }
    while((pos = comand.find(":")) != std::string::npos){
        v.push_back(comand.substr(0,pos));
        comand.erase(0,pos + 1);
        i++;
        if(i==3){
            break;
        }
    }
    v.push_back(comand);

}

