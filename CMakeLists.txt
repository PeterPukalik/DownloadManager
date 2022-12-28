cmake_minimum_required(VERSION 3.18)
project(DownloadManager)

set(CMAKE_CXX_STANDARD 14)

add_executable(DownloadManager main.cpp source/server.cpp source/client.cpp header/server.h header/client.h)
