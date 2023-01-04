//
// Created by puki on 3.1.2023.
//

#ifndef DOWNLOADMANAGER_DATA_H
#define DOWNLOADMANAGER_DATA_H


#include <string>

class Data {


private:
public:
    pthread_mutex_t *getMutex() ;

    pthread_cond_t *getCondSpravcaPriority() ;

    pthread_cond_t *getCondVlakno() ;

    int* getTotalSize() ;

    void setTotalSize(int totalSize);

private:
    std::string protocol;
    std::string web;
    std::string path;
    std::string name;
    long long index;
    int priority;
    std::string time;
    bool stop;
    int bufferPriority;
    int startPoint;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_spravcaPriority;
    pthread_cond_t* cond_vlakno;
    double allreadyDownloaded;
    int totalSize;
public:
    Data(const std::string &aProtocol, const std::string &web, const std::string &path, const std::string &name,
         long long int index, int priority, const std::string &time, bool stop, int bufferPriority, int startPoint,
         pthread_mutex_t *mutex, pthread_cond_t *condSpravcaPriority, pthread_cond_t *condVlakno,int allreadyDownloaded,int totalSize);


public:
    const std::string &getAProtocol() const;

    void setAProtocol(const std::string &aProtocol);

    const std::string &getWeb() const;

    void setWeb(const std::string &web);

    const std::string &getPath() const;

    void setPath(const std::string &path);

    const std::string &getName() const;

    void setName(const std::string &name);

    double* getAllreadyDownloaded();

    void setAllreadyDownloaded(int allreadyDownloaded);

    long long int getIndex() const;

    void setIndex(long long int index);

    int getPriority() const;

    void setPriority(int priority);

    const std::string &getTime() const;

    void setTime(const std::string &time);

    bool& isStop();

    void setStop(bool stop);

    int getBufferPriority() const;

    void setBufferPriority(int bufferPriority);

    int getStartPoint() const;

    void setStartPoint(int startPoint);
};


#endif //DOWNLOADMANAGER_DATA_H
