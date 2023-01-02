//
// Created by puki on 1.1.2023.
//

#include <ctime>
#include <string>
#include <iostream>


//bool compareDates(std::string month,std::string day,std::string hour,std::string minute,std::string partOfTheDay){
int compareDates(char* date){
    std::time_t t_input,t_now;
    t_now = std::time(nullptr);
    struct std::tm tm;
    strptime(date, " %b %d %T %Y",&tm);
    t_input = mktime(&tm);
    printf("%s",ctime(&t_input));
    printf("%s",ctime(&t_now));

    return std::difftime(t_now, t_input);
}

int isTimeToDownload(char* date){
    if(!compareDates(date)){
        isTimeToDownload(date);
    }else{
        return 1;
    }
}

time_t returnTime(char * date){
    std::time_t t_input;
    struct std::tm tm;
    strptime(date, "%b %d %T",&tm);
    return  mktime(&tm);

}
