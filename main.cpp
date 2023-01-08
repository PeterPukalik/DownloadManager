
#include "header/Ftp.h"
#include "header/Ftps.h"
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
#include <filesystem>

#define MAX_NUMBER_OF_THREADS 2



std::string getLastLine(std::ifstream& in)
{
    std::string line;
    while (in >> std::ws && std::getline(in, line)) // skip empty lines
        ;

    return line;
}

//TODO: priority not working properly
//void *managerPriority(void * sdata) {
//
//    std::vector<Data*>* data = (std::vector<Data*>*) sdata;
//    std::cout << "som v managerovi managerPriority\n";
////    pthread_mutex_lock(data->at(0)->getMutex());
////    pthread_cond_wait(data->at(0)->getCondSpravcaPriority(),data->at(0)->getMutex());
////    pthread_mutex_unlock(data->at(0)->getMutex());
//
//
//    if(data->size() > MAX_NUMBER_OF_THREADS){
//
//        std::cout << "som v managerovi za condom\n";
//        int numberOfActiveThread = 0;
//
//        for (int i = 0; i < data->size(); i++) {
//            if(data->at(i)->getFlag() == 1 || data->at(i)->getFlag() == 0){
//                numberOfActiveThread++;
//            }
//            if( numberOfActiveThread > MAX_NUMBER_OF_THREADS ){
//                break;
//            }
//        }
//        if( numberOfActiveThread > MAX_NUMBER_OF_THREADS ) {
//            int worstPriority = INT16_MIN;
//            long long int worstPriorityID = 0;
//            for (int i = 0; i < data->size(); i++) {
//                if (data->at(i)->getPriority() > worstPriority && data->at(i)->getFlag() != 4) {
//                    worstPriority = data->at(i)->getPriority();
//                    worstPriorityID = i;
//                }
//            }
//            std::cout << "lockol som mutex managerPriority\n";
//            pthread_mutex_lock(data->at(0)->getMutex());
//            data->at(worstPriorityID)->setStop(true);
//            data->at(worstPriorityID)->setFlag(4);
//            pthread_mutex_unlock(data->at(0)->getMutex());
//            std::cout << "unlockol som mutex managerPriority\n";
//            std::cout << "stopujem vlakno  managerPriority\n";
//        }
//        std::cout << "koncim v manazerovi managerPriority\n";
//
//    }
//   // std::cout << "boradcast pre download \n";
//    //pthread_cond_broadcast(data->at(0)->getCondVlakno());
//    return nullptr;
//}



void *downloand(void * sdata) {

    Data* data = (Data*) sdata;
    std::cout << "som v downloade \n";
    std::cout << "idem stahovat \n";
    std::string time = data->getTime();
    //TODO
    //osetrit ze sa vyvori vlakno a moze sa stat ze nebude cas a vypne sa program
    int seconds = compareDates(time);
    while(true && data->isTotalStop()){
        seconds = compareDates(time);
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


    if (seconds >= 0) {
        std::cout << "zacinam download \n";
        if (data->getAProtocol() == "http") {
            http(data);
        } else if (data->getAProtocol() == "https") {
            https(data);
        } else if (data->getAProtocol() == "ftp") {

            ftp(data);
        } else if (data->getAProtocol() == "ftps") {
            ftps(data);
        }
    }
//    pthread_mutex_lock(data->getMutex());
//    pthread_cond_wait(data->getCondVlakno(),data->getMutex());
//    pthread_mutex_unlock(data->getMutex());


    return nullptr;
}

void *managerResume(void * sdata) {
    std::vector<Data*>* data = (std::vector<Data*>*) sdata;

    for (int i = 0; i < data->size(); i++) {
        if(data->at(i)->getFlag()==2){
            std::cout << "lockol som mutex managerResume\n";
            pthread_mutex_lock(data->at(i)->getMutex());
            data->at(i)->setFlag(0);
            data->at(i)->setStartPoint((int)data->at(i)->getAllreadyDownloaded());
            std::cout << "unlockol som mutex managerResume\n";
            pthread_mutex_unlock(data->at(i)->getMutex());
            pthread_t resumedThred;
            pthread_create(&resumedThred, nullptr,&downloand,data->at(i));
            pthread_detach(resumedThred);
        }
    }
    return nullptr;
}

void *managerResumePriority(void * sdata) {
    std::vector<Data *> *data = (std::vector<Data *> *) sdata;
    while (*(data->at(0)->isTotalStop())){
        //std::cout << "start       managerResumePriority\n";
            int numberOfActiveThread = 0;
            for (int i = 0; i < data->size(); i++) {
                if (data->at(i)->getFlag() == 1 || data->at(i)->getFlag() == 0) {
                    numberOfActiveThread++;
                }
                if (numberOfActiveThread > MAX_NUMBER_OF_THREADS) {
                    break;
                }
            }
            //std::cout << "lockol som mutex managerResumePriority\n";
            //if stahuje sa menej ako 2
            if (numberOfActiveThread < MAX_NUMBER_OF_THREADS) {
                for (int i = 0; i < data->size(); i++) {
                    if (data->at(i)->getFlag() == 4) {
                        std::cout << "lockol som mutex managerResumePriority\n";
                        pthread_mutex_lock(data->at(i)->getMutex());
                        data->at(i)->setStartPoint((int) data->at(i)->getAllreadyDownloaded());
                        data->at(i)->setFlag(0);
                        data->at(i)->setStop(false);
                        std::cout << "unlockol som mutex managerResumePriority\n";
                        pthread_mutex_unlock(data->at(i)->getMutex());
                        pthread_t resumedThred;
                        pthread_create(&resumedThred, nullptr, &downloand, data->at(i));
                        pthread_detach(resumedThred);
                    }
                }
            }
            if (numberOfActiveThread > MAX_NUMBER_OF_THREADS) {
                int worstPriority = INT16_MIN;
                long long int worstPriorityID = 0;
                for (int i = 0; i < data->size(); i++) {
                    if (data->at(i)->getPriority() > worstPriority && data->at(i)->getFlag() != 4) {
                        worstPriority = data->at(i)->getPriority();
                        worstPriorityID = i;
                    }
                }
                std::cout << "lockol som mutex managerPriority\n";
                pthread_mutex_lock(data->at(0)->getMutex());
                data->at(worstPriorityID)->setStop(true);
                data->at(worstPriorityID)->setFlag(4);
                data->at(worstPriorityID)->setStartPoint((int)data->at(worstPriorityID)->getAllreadyDownloaded());
                pthread_mutex_unlock(data->at(0)->getMutex());
                std::cout << "unlockol som mutex managerPriority\n";
                std::cout << "stopujem vlakno  managerPriority\n";
            }

            //if stahuje sa viac ako 2
        //std::cout << "idem spat       managerResumePriority\n";
            sleep(5);


    }
    return nullptr;
}


int main(int argc, char* argv[]) {

    bool running = true;
    long long index=0;

    if (std::filesystem::exists("history.txt")) {
        std::ifstream file("history.txt");
        std::string line = getLastLine(file);
        int i = 4;
        std::string number;
        while (line.at(i) != ' ' ){
            number = number + line.at(i);
            i++;
        }
        index= std::stoi(number) +1;
        file.close();
    } else {
        // File does not exist
        std::ifstream file("history.txt");
        file.close();
    }




    bool managerExists = false;
    pthread_mutex_t mutex;

    pthread_t managerR;
    pthread_t managerResumeAftefPriorityStop;
    pthread_t vlakna;

    int numberOfActiveDownloads = 0;

    pthread_mutex_init(&mutex, nullptr);


    std::vector<Data*> data;
    std::vector<std::string> parameters{};





    while(running){                                                                                                      //http pukalik.sk /pos/dog.jpeg dog 1
        std::cout << std::endl << "choose you command \nfor exit type \"exit\"\n(download,help,exit,stop,resume,status,manager)" << std::endl;//http pukalik.sk /pos/pos_big.zip testStop 1 // 2023 1 3 19:44:00
        // http pukalik.sk /pos/big_file.zip testThread1 1 // 2023 1 4 13:00:01
        // https frcatel.fri.uniza.sk /users/beerb/ma1/ma-1.pdf ma-1 1
        // https frcatel.fri.uniza.sk /users/beerb/ma1/prednasky/integraly.pdf integraly 1
        //https://frcatel.fri.uniza.sk/users/beerb/ma1/
        // https github.com /pytorch/pytorch.git test 1
        ///https jetbrains.com /pycharm/download/download-thanks.html?platform=linux pycharm1 1

        //https speed.hetzner.de /1GB.bin 1gbfilepriorty10 10
        //https speed.hetzner.de /100MB.bin 1gbfilepriorty10 10
        //

        //https github.com /pytorch/pytorch/archive/refs/tags/v1.13.1.tar.gz pytorch 1
        //https codeload.github.com /pytorch/pytorch/tar.gz/refs/tags/v1.13.1 pytorch 1
        // 2023 1 5 13:03:01
        //https frcatel.fri.uniza.sk /users/beerb/skusky/2022.2023/pisomka1.pdf pisomka1 1

        //ftp ftp.funet.fi /pub/languages/c++/old/Motif1.1-g++.tar.Z funckneftp22 2
        // /pub/graphics/opensourcegaming/test_textures/Parallaxed_Qarl.7z
        // ftp ftp.funet.fi /pub/languages/c++/old/Motif1.1-g++.tar.Z funckneftp22 2
        // ftp ftp.funet.fi /pub/README funckneftp22 2


        // ftps test.rebex.net /pub/example/readme.txt testftps 1 //demo/password
        // ftp test.rebex.net /pub/example/readme.txt testftp 1 //hocico

        //http pukalik.sk /pos/CoffeeHouse.rtf dokumentacia10 10
        //http pukalik.sk /pos/CoffeeHouse.rtf dokumentacia10 10
        //http pukalik.sk /pos/video.mp4 video10 10
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
            //http pukalik.sk /pos/dog.jpeg testPriority1 1
            //http pukalik.sk /pos/dog.jpeg testPriority2 2
            //http pukalik.sk /pos/dog.jpeg testPriority3 3
            //http pukalik.sk /pos/dog.jpeg testPriority4 4
            //2023 1 7 18:44:31
            //std::cin.ignore();
            getline(std::cin, command);
            parameters.push_back(command);
            pthread_mutex_lock(&mutex);
            data.push_back(new Data(parameters.at(0),parameters.at(1),parameters.at(2),parameters.at(3),
                                    index,std::stoi(parameters.at(4)),parameters.at(5),false,0,
                                    &mutex,0,0,0,&running));
            index++;
            pthread_mutex_unlock(&mutex);


            if(parameters.at(0)== "ftp" || parameters.at(0)== "ftps") {
                std::string input;
                std::cout << "INPUT USER NAME FOR FTP: \n";
                std::getline(std::cin, input);
                pthread_mutex_lock(&mutex);
                data.at(data.size() - 1)->setFtpUser(input);
                pthread_mutex_unlock(&mutex);
                std::cout << "INPUT PASS FOR FTP: \n";
                std::getline(std::cin, input);
                pthread_mutex_lock(&mutex);
                data.at(data.size() - 1)->setFtpPass(input);
                pthread_mutex_unlock(&mutex);
            }
            parameters.clear();
            pthread_create(&vlakna, nullptr,&downloand,data.at(data.size()-1));

            pthread_detach(vlakna);


        }

        else if(command == "stop"){
            std::cout << "som v stop"<< std::endl;

            std::cout << "som v mutexe"<< std::endl;
            for (int i = 0; i < data.size(); i++) {

                if(data.at(i)->getFlag() == 1){
                    pthread_mutex_lock(&mutex);
                    data.at(i)->setStop(true);
                    pthread_mutex_unlock(&mutex);
                }
            }

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
                pthread_detach(managerR);

            }
        }
        else if(command == "status"){
            for (int i = 0; i < data.size(); ++i) {
                std::cout << data.at(i)->getName() << " file has total size of: " << data.at(i)->getTotalSize() << " B and downloaded is "<< (double)(data.at(i)->getAllreadyDownloaded()/ data.at(i)->getTotalSize())*100<< " % FLAG: "<< data.at(i)->getFlag() <<"\n";

            }
        }
        else if(command == "manager"){
            std::cout << "you choose manager" << std::endl;
            //std::cout << "http(0) pukalik.sk(1) /pos/dog.jpeg(2) (name)(3) priority(4)" << std::endl;
            std::cout << "comands: ls,mkdir,mv" << std::endl;
            std::cin.ignore();
            getline(std::cin, command);
            int output = system(command.c_str());
            if (output != 0) {
                std::cerr << ("Error executing command\n");
            }

        }
        else if(command =="history"){
            std::ifstream input_file("history.txt");
            std::string line;
            while (std::getline(input_file, line)) {
                std::cout << line << std::endl;
            }
            input_file.close();

            // Seek to the end of the file
        }
        else if(command == "exit"){
            std::cout << "this will terminate all your current downlaods: type Y or N :  ";
            std::cin >> command;
            if (command == "Y" || command == "y"){
                for (int i = 0; i < data.size(); i++) {
                    if(data.at(i)->getFlag() == 1){
                        pthread_mutex_lock(&mutex);
                        data.at(i)->setStop(true);
                        pthread_mutex_unlock(&mutex);
                    }
                }
                std::ofstream outdata;
                outdata.open("history.txt", std::ios_base::app);
                if(!outdata){
                    std::cerr << "Error : file could not be opened" << std::endl;
                    return 1;
                }
                for (int i = 0; i < data.size(); i++) {
                    outdata <<"ID: "<<data.at(i)->getIndex() <<" "<< data.at(i)->getName() << " stiahnute: " << (double)(data.at(i)->getAllreadyDownloaded()/ data.at(i)->getTotalSize())*100 << " %" << std::endl ;
                }
                outdata.close();
                running = false;
            }else{

            }

        }
        if(!data.empty() && !managerExists){
            pthread_create(&managerResumeAftefPriorityStop, nullptr,&managerResumePriority,&data);
            managerExists = true;
        }

    }
    //TODO join
    if(data.size() > 1)
        pthread_join(managerResumeAftefPriorityStop,nullptr);


    for (int i = 0; i < data.size(); i++) {
        delete data.at(i);
    }
    data.clear();
    //TODO join
    //pthread_join(vlakna,nullptr);
    //pthread_join(managerP, nullptr);
    //pthread_join(managerResumeAftefPriorityStop, nullptr);
   // pthread_join(managerR, nullptr);



    pthread_mutex_destroy(&mutex);

    return 0;

}


