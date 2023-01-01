#include <cstdio>
#include "header/http.h"
#include <cstring>
#include <string>
#include <iostream>
#include <pthread.h>


typedef struct ITEM{
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_spravca;
    pthread_cond_t* cond_vlakno;
    std::string web;
    std::string path;
    std::string name;
    int index;

}ITEM;


void *manager(void *) {
    return nullptr;
}

void *downloand(void * sdata) {
    ITEM* data = (ITEM*) sdata;
    pthread_mutex_lock(data->mutex);
    int index = data->index++;
    pthread_mutex_unlock(data->mutex);
    http("pukalik.sk","/pos/dog.jpeg","testBigFile" + std::to_string(index),index);
    return nullptr;
}

int main(int argc, char* argv[]) {

    bool running = true;
    pthread_mutex_t mutex;
    pthread_cond_t cond_spravca, cond_vlakno;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond_spravca, nullptr);
    pthread_cond_init(&cond_vlakno, nullptr);

    ITEM data = {&mutex,&cond_spravca,&cond_vlakno,"pukalik.sk","/pos/dog.jpeg","dog",1};
    while(running){
        std::cout << std::endl << "choose you comand \nfor exit type \"exit\"" << std::endl;
        std::string comand;
        std::cin >> comand;
        if(comand == "http"){
            std::cout << "you choose http" << std::endl;
            pthread_t spravca;
            pthread_t vlakna[5];
            pthread_create(&spravca, nullptr,&manager,&data);
            for (int i = 0; i < 5; ++i) {
                pthread_create(&vlakna[i], nullptr,&downloand,&data);
            }
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
