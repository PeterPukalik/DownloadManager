//
// Created by gerat on 5. 1. 2023.
//

#ifndef DOWNLOADMANAGER_FTP_H
#define DOWNLOADMANAGER_FTP_H

#include <string>
#include <vector>
#include "Data.h"

int ftp(Data* data);
std::vector<std::string> split_string(const std::string& s, char delimiter);
#endif //DOWNLOADMANAGER_FTP_H
