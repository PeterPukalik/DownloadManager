cmake_minimum_required(VERSION 3.18)
project(DownloadManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(DownloadManager main.cpp source/server.cpp source/client.cpp header/server.h header/client.h header/http.h source/http.cpp source/manager.cpp header/manager.h source/Parser.cpp header/Parser.h)

target_link_libraries(DownloadManager pthread)
