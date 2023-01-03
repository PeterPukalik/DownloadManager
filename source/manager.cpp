//
// Created by puki on 1.1.2023.
//
#include "../header/manager.h"
#include <ctime>
#include <string>
#include <iostream>




//bool compareDates(std::string month,std::string day,std::string hour,std::string minute,std::string partOfTheDay){
int compareDates(std::string date){
    std::time_t t_input,t_now;
    t_now = std::time(nullptr);
    std::vector<std::string> parameters{};
    struct std::tm tm={0};
    Parser::timeParser(parameters,date);
    //i=0//rok i=1//mesiac i=2//deni=3 meni sa deliemeter //hodina //minuta //sec
    tm.tm_year = std::stoi(parameters.at(0))-1900;
    tm.tm_mon = std::stoi(parameters.at(1))-1;
    tm.tm_mday = std::stoi(parameters.at(2));
    tm.tm_hour = std::stoi(parameters.at(3));
    tm.tm_min = std::stoi(parameters.at(4));
    tm.tm_sec = std::stoi(parameters.at(5));
//    tm.tm_year = 123;//od 1900
//    tm.tm_mon = 0;//mesiac jan = 0
//    tm.tm_mday = 2;
    t_input = mktime(&tm);
    printf("%s",ctime(&t_input));
    printf("%s",ctime(&t_now));

    return std::difftime(t_now, t_input);
}



