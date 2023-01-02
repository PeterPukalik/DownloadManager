#include "header/http.h"
#include "header/manager.h"
#include <cstring>
#include <string>
#include <iostream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>


typedef struct ITEM{
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_spravca;
    pthread_cond_t* cond_vlakno;
    std::string web;
    std::string path;
    std::string name;
    int index;
    std::string time;

}ITEM;


void *manager(void *) {
    return nullptr;
}

void *downloand(void * sdata) {
    ITEM* data = (ITEM*) sdata;
    pthread_mutex_lock(data->mutex);
    int index = data->index++;
    char* time = new char[data->time.length() + 1];
    strcpy(time,data->time.c_str());
    pthread_mutex_unlock(data->mutex);
    while(1){
        if (compareDates(time) > 0) {
            break;
        } else {
            std::cout << "waiting \n";
            sleep(5);

        }
    }
    http("pukalik.sk", "/pos/dog.jpeg", "dog");
    return nullptr;
}

int main(int argc, char* argv[]) {

    bool running = true;
    pthread_mutex_t mutex;
    pthread_cond_t cond_spravca, cond_vlakno;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond_spravca, nullptr);
    pthread_cond_init(&cond_vlakno, nullptr);

    ITEM data = {&mutex,&cond_spravca,&cond_vlakno,"pukalik.sk","/pos/dog.jpeg","dog",1,"jan 2 12:54:22 2023"};
    while(running){
        std::cout << std::endl << "choose you comand \nfor exit type \"exit\"" << std::endl;
        std::string comand;
        std::cin >> comand;
        if(comand == "http"){
            std::cout << "you choose http" << std::endl;
            pthread_t spravca;
            pthread_t vlakna[1];
            pthread_create(&spravca, nullptr,&manager,&data);
            for (int i = 0; i < 1; ++i) {
                pthread_create(&vlakna[i], nullptr,&downloand,&data);
            }
        }
        if(comand == "time"){
            std::cout << std::endl << "time date and time in format(Mar 1 05:10:00 )" << std::endl;
            std::cin.ignore();
            getline(std::cin,comand);
            char* time = new char[comand.length() + 1];
            strcpy(time,comand.c_str());
            std::cout << compareDates(time);
            delete[] time;
        }


        else if(comand == "exit"){
            running = false;

        }
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_spravca);
    pthread_cond_destroy(&cond_vlakno);
    return 0;

}
