//
// Created by puki on 3.1.2023.
//

#include "../header/Data.h"

const std::string &Data::getAProtocol() const {
    return protocol;
}

void Data::setAProtocol(const std::string &aProtocol) {
    protocol = aProtocol;
}

const std::string &Data::getWeb() const {
    return web;
}

void Data::setWeb(const std::string &web) {
    Data::web = web;
}

const std::string &Data::getPath() const {
    return path;
}

void Data::setPath(const std::string &path) {
    Data::path = path;
}

const std::string &Data::getName() const {
    return name;
}

void Data::setName(const std::string &name) {
    Data::name = name;
}

long long int Data::getIndex() const {
    return index;
}

void Data::setIndex(long long int index) {
    Data::index = index;
}

int Data::getPriority() const {
    return priority;
}

void Data::setPriority(int priority) {
    Data::priority = priority;
}

const std::string &Data::getTime() const {
    return time;
}

void Data::setTime(const std::string &time) {
    Data::time = time;
}

bool& Data::isStop(){
    return stop;
}

void Data::setStop(bool stop) {
    Data::stop = stop;
}

int Data::getBufferPriority() const {
    return bufferPriority;
}

void Data::setBufferPriority(int bufferPriority) {
    Data::bufferPriority = bufferPriority;
}

int Data::getStartPoint() const {
    return startPoint;
}

void Data::setStartPoint(int startPoint) {
    Data::startPoint = startPoint;
}

Data::Data(const std::string &aProtocol, const std::string &web, const std::string &path, const std::string &name,
           long long int index, int priority, const std::string &time, bool stop, int bufferPriority, int startPoint,
           pthread_mutex_t *mutex, pthread_cond_t *condSpravcaPriority, pthread_cond_t *condVlakno,int allreadyDownloaded,int totalSize)
           : protocol(aProtocol), web(web), path(path), name(name), index(index), priority(priority), time(time), stop(stop),
            bufferPriority(bufferPriority),
            startPoint(startPoint),
            mutex(mutex),
            cond_spravcaPriority(condSpravcaPriority),cond_vlakno(condVlakno),allreadyDownloaded(allreadyDownloaded),totalSize(totalSize) {


}

pthread_mutex_t *Data::getMutex(){
    return mutex;
}

pthread_cond_t *Data::getCondSpravcaPriority()  {
    return cond_spravcaPriority;
}

pthread_cond_t *Data::getCondVlakno()  {
    return cond_vlakno;
}

double* Data::getAllreadyDownloaded()  {
    return &allreadyDownloaded;
}

void Data::setAllreadyDownloaded(int allreadyDownloaded) {
    Data::allreadyDownloaded = allreadyDownloaded;
}

int* Data::getTotalSize()  {
    return &totalSize;
}

void Data::setTotalSize(int totalSize) {
    Data::totalSize = totalSize;
}

