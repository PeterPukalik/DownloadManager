#include "header/Http.h"
#include "header/Manager.h"
#include "header/Parser.h"
#include "header/Data.h"
#include <cstring>
#include <string>
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <map>



void *managerPriority(void * sdata) {
    std::vector<Data*>* data = (std::vector<Data*>*) sdata;



    return nullptr;
}



void *downloand(void * sdata) {

    Data* data = (Data*) sdata;
//    pthread_mutex_lock(data->getMutex());
//    pthread_cond_wait(data->getCondSpravcaPriority(),data->getMutex());
//    pthread_mutex_unlock(data->getMutex());
    std::string time = data->getTime();
    while(true){
        int seconds = compareDates(time);
        //std::cout << seconds << std::endl;
        if (seconds >= 0) {
            break;
        } else {
            //std::cout << "waiting \n";
            sleep(5);
        }
    }
    //http(data->getWeb(),data->getPath(),data->getName(),data->getStartPoint(),&data->isStop(),data->getIndex());
    http(data->getWeb(),data->getPath(),data->getName(),data->getStartPoint(),data->isStop(),data->getIndex(),data->getAllreadyDownloaded(),data->getTotalSize());

    return nullptr;
}


int main(int argc, char* argv[]) {

    bool running = true;
    std::ofstream ("resume.txt");
    bool stop = false;
    long long index=0;
    pthread_mutex_t mutex;
    pthread_cond_t  cond_vlakno,cond_spravcaPriority;
    pthread_t managerP;

    int thread;

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond_spravcaPriority, nullptr);
    pthread_cond_init(&cond_vlakno, nullptr);

    std::vector<Data*> data;
    std::vector<std::string> parameters{};

    std::vector<pthread_t> vectorOfThreads;
    pthread_create(&managerP, nullptr,&managerPriority,&data);

    while(running){                                                                                                     //http pukalik.sk /pos/dog.jpeg dog 1
        std::cout << std::endl << "choose you command \nfor exit type \"exit\"\n(http,https,ftp,ftps,help)" << std::endl;//http pukalik.sk /pos/pos_big.zip testStop 1 // 2023 1 3 19:44:00
        // http pukalik.sk /pos/big_file.zip testThread1 1 // 2023 1 4 13:00:01
        std::string command;
        std::cin >> command;
        if(command == "download"){
            std::cout << "you choose download" << std::endl;
            //std::cout << "http(0) pukalik.sk(1) /pos/dog.jpeg(2) (name)(3) priority(4)" << std::endl;
            std::cout << "http pukalik.sk /pos/dog.jpeg dog 1" << std::endl;
            std::cin.ignore();
            getline(std::cin, command);
            Parser::setter(parameters, command);
            //std::cout << "enter time(2023 {1=(jan),2=(feb)...} 2 20:20:20)" << std::endl;
            std::cout << "2023 1 3 19:44:00" << std::endl; //2023 1 3 21:02:00

            //std::cin.ignore();
            getline(std::cin, command);
            parameters.push_back(command);
            pthread_mutex_lock(&mutex);
            data.push_back(new Data(parameters.at(0),parameters.at(1),parameters.at(2),parameters.at(3),
                                    index,std::stoi(parameters.at(4)),parameters.at(5),stop,3,0,
                                    &mutex,&cond_spravcaPriority,&cond_vlakno,0,0));
            index++;
            pthread_mutex_unlock(&mutex);
            parameters.clear();
            pthread_t vlakna;
            pthread_create(&vlakna, nullptr,&downloand,data.at(data.size()-1));


        }

        if(command == "stop"){
            pthread_mutex_lock(&mutex);
            stop = true;
            pthread_mutex_unlock(&mutex);
        }
        if(command == "resume"){
            pthread_mutex_lock(&mutex);
            stop = false;
            pthread_mutex_unlock(&mutex);
        }
        if(command == "status"){
            for (int i = 0; i < data.size(); ++i) {
                std::cout << data.at(i)->getName() << " file has total size of: " << *data.at(i)->getTotalSize() << " B and downloaded is "<< (double)(*data.at(i)->getAllreadyDownloaded()/ *data.at(i)->getTotalSize())*100<< " %\n";

            }
        }


        else if(command == "exit"){
            running = false;

        }
    }
    //TODO join
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_spravcaPriority);
    pthread_cond_destroy(&cond_vlakno);
    return 0;

}
