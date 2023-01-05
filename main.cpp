#include "header/Http.h"
#include "header/Https.h"




#include "header/Manager.h"
#include "header/Parser.h"
#include "header/Data.h"

#include <string>
#include <iostream>
#include <pthread.h>

#include <unistd.h>
#include <vector>
#include <fstream>
#include <map>

#define MAX_NUMBER_OF_THREADS 3


//TODO: priority not working properly
void *managerPriority(void * sdata) {

    std::vector<Data*>* data = (std::vector<Data*>*) sdata;
    std::cout << "som v managerovi \n";
//    pthread_mutex_lock(data->at(0)->getMutex());
//    pthread_cond_wait(data->at(0)->getCondSpravcaPriority(),data->at(0)->getMutex());
//    pthread_mutex_unlock(data->at(0)->getMutex());


    if(data->size() > MAX_NUMBER_OF_THREADS){

        std::cout << "som v managerovi za condom\n";
        int numberOfActiveThread = 0;

        for (int i = 0; i < data->size(); i++) {
            if(data->at(i)->getFlag() == 1 || data->at(i)->getFlag() == 0){
                numberOfActiveThread++;
            }
            if( numberOfActiveThread > MAX_NUMBER_OF_THREADS ){
                break;
            }
        }
        if( numberOfActiveThread > MAX_NUMBER_OF_THREADS ) {
            int worstPriority = INT16_MIN;
            long long int worstPriorityID = 0;
            for (int i = 0; i < data->size(); i++) {
                if (data->at(i)->getPriority() > worstPriority && data->at(i)->getFlag() != 4) {
                    worstPriority = data->at(i)->getPriority();
                    worstPriorityID = i;
                }
            }
            pthread_mutex_lock(data->at(0)->getMutex());
            data->at(worstPriorityID)->setStop(true);
            data->at(worstPriorityID)->setFlag(4);
            pthread_mutex_unlock(data->at(0)->getMutex());
            std::cout << "stopujem vlakno  \n";
        }
        std::cout << "koncim v manazerovi \n";

    }
    std::cout << "boradcast pre download \n";
    pthread_cond_broadcast(data->at(0)->getCondVlakno());
    return nullptr;
}



void *downloand(void * sdata) {

    Data* data = (Data*) sdata;
    std::cout << "som v downloade \n";
    pthread_cond_signal(data->getCondSpravcaPriority());

//    pthread_mutex_lock(data->getMutex());
//    pthread_cond_wait(data->getCondVlakno(),data->getMutex());
//    pthread_mutex_unlock(data->getMutex());

    std::cout << "idem stahovat \n";
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
    //http(data->getWeb(),data->getPath(),data->getName(),data->getStartPoint(),data->isStop(),data->getIndex(),data->getAllreadyDownloaded(),data->getTotalSize(),data->getFlag());
    std::cout << "zacinam download \n";
    if(data->getAProtocol() == "http"){
        http(data);
    }
    else if(data->getAProtocol() == "https"){
        https(data);
    }
    else if(data->getAProtocol() == "ftp"){

    }
    else if(data->getAProtocol() == "ftps"){

    }


    return nullptr;
}

void *managerResume(void * sdata) {
    std::vector<Data*>* data = (std::vector<Data*>*) sdata;
    for (int i = 0; i < data->size(); i++) {
        if(data->at(i)->getFlag()==2){
            pthread_mutex_lock(data->at(i)->getMutex());
            data->at(i)->setFlag(0);
            data->at(i)->setStartPoint((int)data->at(i)->getAllreadyDownloaded());
            pthread_mutex_unlock(data->at(i)->getMutex());
            pthread_t resumedThred;
            pthread_create(&resumedThred, nullptr,&downloand,data->at(i));
        }
    }
    return nullptr;
}


int main(int argc, char* argv[]) {

    bool running = true;
    std::ofstream ("resume.txt");

    long long index=0;
    bool managerExists = false;
    pthread_mutex_t mutex;
    pthread_cond_t  cond_vlakno,cond_spravcaPriority;
    pthread_t managerP;
    pthread_t managerR;
    pthread_t vlakna;

    int thread;

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond_spravcaPriority, nullptr);
    pthread_cond_init(&cond_vlakno, nullptr);

    std::vector<Data*> data;
    std::vector<std::string> parameters{};

    std::vector<pthread_t> vectorOfThreads;
//    Ssl_connection con("https://google.com");
//    con.get_sec();




    while(running){                                                                                                      //http pukalik.sk /pos/dog.jpeg dog 1
        std::cout << std::endl << "choose you command \nfor exit type \"exit\"\n(download,help,exit,stop,resume,status)" << std::endl;//http pukalik.sk /pos/pos_big.zip testStop 1 // 2023 1 3 19:44:00
        // http pukalik.sk /pos/big_file.zip testThread1 1 // 2023 1 4 13:00:01
        // https frcatel.fri.uniza.sk /users/beerb/ma1/ma-1.pdf ma-1 1
        //https://frcatel.fri.uniza.sk/users/beerb/ma1/
        // https github.com /pytorch/pytorch.git test 1
        ///https jetbrains.com /pycharm/download/download-thanks.html?platform=linux pycharm1 1

        //https speed.hetzner.de /1GB.bin 1gbfilepriorty10 10

        //https github.com /pytorch/pytorch/archive/refs/tags/v1.13.1.tar.gz pytorch 1
        //https codeload.github.com /pytorch/pytorch/tar.gz/refs/tags/v1.13.1 pytorch 1
        // 2023 1 5 13:03:01
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
            std::cout << "2023 1 3 19:44:00    or type \"now\"" << std::endl; //2023 1 3 21:02:00
            //http pukalik.sk /pos/big_file.zip testPriority1 1
            //http pukalik.sk /pos/big_file.zip testPriority2 2
            //http pukalik.sk /pos/big_file.zip testPriority3 3
            //http pukalik.sk /pos/big_file.zip testPriority4 4
            //2023 1 4 22:15:01
            //std::cin.ignore();
            getline(std::cin, command);
            parameters.push_back(command);
            pthread_mutex_lock(&mutex);
            data.push_back(new Data(parameters.at(0),parameters.at(1),parameters.at(2),parameters.at(3),
                                    index,std::stoi(parameters.at(4)),parameters.at(5),false,0,
                                    &mutex,&cond_spravcaPriority,&cond_vlakno,0,0,0));
            index++;
            pthread_mutex_unlock(&mutex);
            parameters.clear();

            pthread_create(&vlakna, nullptr,&downloand,data.at(data.size()-1));

        }

        else if(command == "stop"){
            std::cout << "som v stop"<< std::endl;
            pthread_mutex_lock(&mutex);
            std::cout << "som v mutexe"<< std::endl;
            for (int i = 0; i < data.size(); i++) {
                if(data.at(i)->getFlag() == 1)
                    data.at(i)->setStop(true);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if(command == "resume"){
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < data.size(); i++) {
                data.at(i)->setStop(false);
            }
            pthread_mutex_unlock(&mutex);
            if(data.empty()){
                std::cout << "nothing to resume\n";
            }else{

                pthread_create(&managerR, nullptr,&managerResume,&data);
            }
        }
        else if(command == "status"){
            for (int i = 0; i < data.size(); ++i) {
                std::cout << data.at(i)->getName() << " file has total size of: " << data.at(i)->getTotalSize() << " B and downloaded is "<< (double)(data.at(i)->getAllreadyDownloaded()/ data.at(i)->getTotalSize())*100<< " %\n";

            }
        }
        else if(command == "exit"){
            running = false;
        }
        if(!data.empty() && !managerExists){
            pthread_create(&managerP, nullptr,&managerPriority,&data);
            managerExists = true;
        }

    }
    //TODO join
    for (int i = 0; i < data.size(); i++) {
        delete data.at(i);
    }
    data.clear();
    //pthread_join(vlakna,nullptr);
    //pthread_join(managerP, nullptr);

    //pthread_join(managerR, nullptr);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_spravcaPriority);
    pthread_cond_destroy(&cond_vlakno);
    return 0;

}
