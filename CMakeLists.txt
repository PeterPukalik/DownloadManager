cmake_minimum_required(VERSION 3.18)
project(DownloadManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(DownloadManager main.cpp source/server.cpp source/client.cpp header/server.h header/client.h header/Http.h source/Http.cpp source/Manager.cpp header/Manager.h source/Parser.cpp header/Parser.h source/Data.cpp header/Data.h source/Https.cpp header/Https.h header/Ftp.h source/Ftp.cpp header/Ftps.h source/Ftps.cpp)


target_link_libraries(DownloadManager pthread ssl crypto)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak ")