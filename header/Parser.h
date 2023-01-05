//
// Created by puki on 2.1.2023.
//

#ifndef DOWNLOADMANAGER_PARSER_H
#define DOWNLOADMANAGER_PARSER_H


#include <string>
#include <vector>

class Parser {
public:
    const std::string &getWeb() const;

    const std::string &getPath() const;

    const std::string &getName() const;

    int getPriority() const;

    const std::string &getTime() const;

    static void setter(std::vector<std::string> &v, std::string command);
    static void timeParser(std::vector<std::string> &v, std::string comand);
private:
    std::string protocol;
    std::string web;
    std::string path;
    std::string name;
    int priority;
    std::string time;



};


#endif //DOWNLOADMANAGER_PARSER_H
