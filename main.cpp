#include "header/http.h"
#include "header/manager.h"
#include "header/Parser.h"
#include <cstring>
#include <string>
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <fstream>


typedef struct ITEM{
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_spravcaPriority;
    pthread_cond_t* cond_vlakno;
    std::string protocol;
    std::string web;
    std::string path;
    std::string name;
    int index;
    int priority;
    std::string time;
    bool stop;
    int bufferPriority;

}ITEM;


void *downloand(void * sdata) {
    ITEM* data = (ITEM*) sdata;
    pthread_mutex_lock(data->mutex);
    int index = data->index++;
    std::string time = data->time;

    pthread_mutex_unlock(data->mutex);
    while(1){
        int seconds = compareDates(time);
        std::cout << seconds << std::endl;
        if (seconds > 0) {
            break;
        } else {
            std::cout << "waiting \n";
            sleep(5);
        }
    }
    http("pukalik.sk", "/pos/big_file.zip", data->name,0,data->stop);

    return nullptr;
}

void *managerPriority(void * sdata) {
    return nullptr;
}


int main(int argc, char* argv[]) {

    bool running = true;
    std::ofstream ("resume.txt");
    bool stop = false;

    pthread_mutex_t mutex;
    pthread_cond_t  cond_vlakno,cond_spravcaPriority;
    pthread_t managerP;

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond_spravcaPriority, nullptr);
    pthread_cond_init(&cond_vlakno, nullptr);

    ITEM data = {&mutex,&cond_spravcaPriority,&cond_vlakno,"","","","",0,0,"",stop,3};
    std::vector<std::string> parameters{};

    pthread_create(&managerP, nullptr,&managerPriority,&data);

    while(running){
        std::cout << std::endl << "choose you command \nfor exit type \"exit\"\n(http,https,ftp,ftps,help)" << std::endl;
        std::string command;
        std::cin >> command;
        if(command == "http"){
            std::cout << "you choose http" << std::endl;
            std::cout << "http(0) pukalik.sk(1) /pos/dog.jpeg(2) (name)(3) priority(4)" << std::endl;
            std::cin.ignore();
            getline(std::cin, command);
            Parser::setter(parameters, command);
            std::cout << "enter time(2023 {1=(jan),2=(feb)...} 2 20:20:20)" << std::endl;
            //std::cin.ignore();
            getline(std::cin, command);
            parameters.push_back(command);
            pthread_mutex_lock(data.mutex);
            data.protocol = parameters.at(0);
            data.web = parameters.at(1);
            data.path = parameters.at(2);
            data.name = parameters.at(3);
            data.priority = std::stoi(parameters.at(4));
            data.time = parameters.at(5);
            pthread_mutex_unlock(data.mutex);
            pthread_t vlakna;
            pthread_create(&vlakna, nullptr,&downloand,&data);

        }
//        if(command == "time"){
//            std::cout << std::endl << "time date and time in format(jan 2 20:34:00 2023)" << std::endl;
//            std::cin.ignore();//kvoli 2 endl nad tym
//            getline(std::cin, command);
//            std::cout << compareDates(command);
//        }
        if(command == "stop"){
            pthread_mutex_lock(data.mutex);
            data.stop = true;
            pthread_mutex_unlock(data.mutex);
        }
        if(command == "resume"){
            pthread_mutex_lock(data.mutex);
            data.stop = false;
            pthread_mutex_unlock(data.mutex);
        }


        else if(command == "exit"){
            running = false;

        }
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_spravcaPriority);
    pthread_cond_destroy(&cond_vlakno);
    return 0;

}
