//
// Created by pukal on 29. 12. 2022.
//

#ifndef DOWNLOADMANAGER_HTTP_H
#define DOWNLOADMANAGER_HTTP_H

#include <string>
#include "Data.h"

//int http(std::string web, std::string path, std::string name,int startPoint,bool *stop,long long index, double *allreadyDownloaded,int *totalSize,int* flag);
int http(Data* data);
#endif //DOWNLOADMANAGER_HTTP_H
