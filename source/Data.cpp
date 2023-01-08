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

bool Data::isStop(){
    return stop;
}

void Data::setStop(bool stop) {
    Data::stop = stop;
}


int Data::getStartPoint() const {
    return startPoint;
}

void Data::setStartPoint(int startPoint) {
    Data::startPoint = startPoint;
}

Data::Data(const std::string &aProtocol, const std::string &web, const std::string &path, const std::string &name,
           long long int index, int priority, const std::string &time, bool stop, int startPoint,
           pthread_mutex_t *mutex, int allreadyDownloaded,int totalSize,int flag,bool* totalStop)
           : protocol(aProtocol), web(web), path(path), name(name), index(index), priority(priority), time(time), stop(stop),
            startPoint(startPoint),mutex(mutex), allreadyDownloaded(allreadyDownloaded),totalSize(totalSize),flag(flag),totalStop(totalStop) {
}

pthread_mutex_t *Data::getMutex(){
    return mutex;
}

double Data::getAllreadyDownloaded(){
    return allreadyDownloaded;
}

void Data::setAllreadyDownloaded(int allreadyDownloaded) {
    Data::allreadyDownloaded = allreadyDownloaded;
}

int Data::getTotalSize()  {
    return totalSize;
}

void Data::setTotalSize(int totalSize) {
    Data::totalSize = totalSize;
}

int Data::getFlag() {
    return flag;
}

void Data::setFlag(int flag) {
    Data::flag = flag;
}

void Data::addAllreadyDownloaded(int plusSize ) {
    allreadyDownloaded += plusSize;
}

bool* Data::isTotalStop()  {
    return totalStop;
}

const std::string &Data::getFtpUser() const {
    return ftpUSER;
}

void Data::setFtpUser(const std::string &ftpUser) {
    ftpUSER = ftpUser;
}

const std::string &Data::getFtpPass() const {
    return ftpPASS;
}

void Data::setFtpPass(const std::string &ftpPass) {
    ftpPASS = ftpPass;
}