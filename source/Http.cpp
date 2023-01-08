//
// sync_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <filesystem>
#include "../header/Http.h"

using boost::asio::ip::tcp;

//int http(std::string web, std::string path, std::string name,int startPoint,bool *stop,long long index, double *allreadyDownloaded,int *totalSize,int* flag)
int http(Data* data)
{
    try
    {
//        if (argc <= 3)
//        {
//            std::cout << "Usage: sync_client <server> <path>\n";
//            std::cout << "Example:\n";
//            std::cout << "pukalik.sk /pos/dog.jpeg dog\n";
//            return 1;
//        }
        boost::asio::io_context io_context;
        int responselength = 0;
        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(data->getWeb(), "http");

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all Data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << data->getPath() << " HTTP/1.0\r\n";
        request_stream << "Host: " << data->getWeb() << "\r\n";
        request_stream << "Range:  bytes=" + boost::lexical_cast<std::string>(data->getStartPoint())+ "-"<< "\r\n";;
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            std::cout << "Invalid response\n";
            return 1;
        }
        if (status_code != 200 && status_code != 206)
        {
            std::cout << "Response returned with status code " << status_code << "\n";
            return 1;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        data->setFlag(1);//started
        while (std::getline(response_stream, header) && header != "\r"){
            if(header.compare(0,16,"Content-Length: ")== 0){
                if(data->getTotalSize() == 0){
                    pthread_mutex_lock(data->getMutex());
                    data->setTotalSize(std::stoi(header.substr(16,header.length())));
                    pthread_mutex_unlock(data->getMutex());
                }

            }
            std::cout << header << "\n";
        }
        //std::cout << "\n";

        // Write whatever content we already have to output.
        std::ofstream outdata;
        //outdata.open((data->getName() + ".dat"), std::ios::out);
//        if (std::filesystem::exists(data->getName() + ".dat")) {
//            int i = 0;
//            while (std::filesystem::exists(data->getName() + ".dat")) {
//                pthread_mutex_lock(data->getMutex());
//                data->setName(data->getName()+ std::to_string(i));
//                pthread_mutex_unlock(data->getMutex());
//
//                i++;
//            }
//
//
//
//        }
        outdata.open((data->getName() + ".dat"), std::ios_base::app);
        if(!outdata){
            std::cerr << "Error : file could not be opened" << std::endl;
            return 1;
        }
        if (response.size() > 0) {
            //std::cout << &response;
            //tu bude premenna kolko mi prislo +=
            pthread_mutex_lock(data->getMutex());
            data->addAllreadyDownloaded(response.size());
            pthread_mutex_unlock(data->getMutex());
            //*allreadyDownloaded += response.size();
            outdata << &response;
        }

        // Read until EOF, writing Data to output as we go.
        boost::system::error_code error;
        while (boost::asio::read(socket, response,boost::asio::transfer_at_least(1), error) && !data->isStop()) {
            pthread_mutex_lock(data->getMutex());
            data->addAllreadyDownloaded(response.size());
            pthread_mutex_unlock(data->getMutex());
            //*allreadyDownloaded += response.size();
            outdata << &response;
        }
        outdata.close();
        if(data->isStop()){
            if(data->getFlag() != 4){
                pthread_mutex_lock(data->getMutex());
                data->setFlag(2);//stoped
                pthread_mutex_unlock(data->getMutex());
            }

        }else{
            pthread_mutex_lock(data->getMutex());
            data->setFlag(3);//finished
            pthread_mutex_unlock(data->getMutex());
        }

        if (error != boost::asio::error::eof)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}