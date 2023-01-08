#include "../header/Data.h"
#include "../header/Ftp.h"
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <vector>

std::vector<std::string> split_string(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int ftp(Data* data) {
    try
    {

    // Create a socket and connect to the FTP server.
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve(data->getWeb(), "21"));


    // Send the FTP USER command to log in.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "USER "<< data->getFtpUser() << "\r\n";
    boost::asio::write(socket, request);

    // Receive the FTP server's response.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
    std::istream response_stream(&response);
    std::string response_string;
    std::getline(response_stream, response_string);
    std::cout << "USER FTP server response: " << response_string << "\n";


    // Send the FTP PASS command to log in.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "PASS "<< data->getFtpPass() << "\r\n";
    boost::asio::write(socket, request);

    // Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "PASS FTP server response: " << response_string << "\n";

    // Get the IP address and port number of the client.
    boost::asio::ip::tcp::endpoint local_endpoint = socket.local_endpoint();
    std::string ip_address = local_endpoint.address().to_string();
    std::string port = std::to_string(local_endpoint.port());


    // Send the FTP PASV command to enter passive mode.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "PASV\r\n";
    boost::asio::write(socket, request);

    // Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "PASV FTP server response: " << response_string << "\n";

    // Extract the IP address and port number from the FTP server's response.

    if (response_string.length() >= 27) {
        response_string = response_string.substr(27, response_string.length() - 1);
    }

    std::vector<std::string> pasv_response_tokens = split_string(response_string, ',');
    std::string data_ip_address = pasv_response_tokens[0] + "." + pasv_response_tokens[1] + "." + pasv_response_tokens[2] + "." + pasv_response_tokens[3];
    int data_port = std::stoi(pasv_response_tokens[4]) * 256 + std::stoi(pasv_response_tokens[5]);

    // Connect to the data port.
    boost::asio::ip::tcp::socket data_socket(io_context);
    boost::asio::ip::tcp::endpoint data_endpoint(boost::asio::ip::make_address(data_ip_address), data_port);
    data_socket.connect(data_endpoint);

    // Send the FTP SIZE command to check the size of a file.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "SIZE " << data->getPath()<<"\r\n";
    boost::asio::write(socket, request);

    // Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "FTP server response: " << response_string << "\n";

    // Extract the size of the file from the FTP server's response.
    if (response_string.substr(0, 3) == "213") {
        std::string file_size = response_string.substr(4);
        std::cout << "File size: " << file_size << " bytes\n";
        pthread_mutex_lock(data->getMutex());
        data->setTotalSize(stoi(file_size));
        pthread_mutex_unlock(data->getMutex());
    } else {
        std::cerr << "Error: invalid response from FTP server\n";
    }
    request.consume(request.size());
    request_stream.clear();
    request_stream << "TYPE I" << "\r\n";
    boost::asio::write(socket, request);
    // Send the FTP REST command to specify the starting position of the file to download.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "REST " << data->getStartPoint() << "\r\n";
    boost::asio::write(socket, request);

// Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "REST FTP server response: " << response_string << "\n";
    // Send the FTP RETR command to retrieve a file.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "RETR " << data->getPath()<<"\r\n";
    boost::asio::write(socket, request);
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "RETR FTP server response: " << response_string << "\n";
    // Open a file to write the data to.
    std::ofstream output_file("testftp.dat", std::ios::binary);
    if (output_file.fail()) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    // Read the FTP server's response and write the data to the file.
    boost::system::error_code error;
//    int responseSizeout = 0;
//    boost::asio::read(data_socket, response, boost::asio::transfer_at_least(1), error);
//    while (response.size() > 0 && !data->isStop()) {
//        responseSizeout+= response.size();
//        pthread_mutex_lock(data->getMutex());
//        data->addAllreadyDownloaded(response.size());
//        pthread_mutex_unlock(data->getMutex());
//        output_file << &response;
//        boost::asio::read(data_socket, response, boost::asio::transfer_at_least(1), error);
//    }
    while (boost::asio::read(data_socket, response, boost::asio::transfer_at_least(1), error) && !data->isStop())
    {
        pthread_mutex_lock(data->getMutex());
        data->addAllreadyDownloaded(response.size());
        pthread_mutex_unlock(data->getMutex());
        output_file << &response;
    }

    output_file.close();
    //std::cerr << responseSizeout << std::endl;
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

    // Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "FTP server response: " << response_string << "\n";

    // Send the FTP QUIT command to log out.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "QUIT\r\n";
    boost::asio::write(socket, request);

    // Receive the FTP server's response.
    response.consume(response.size());
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "FTP server response: " << response_string << "\n";
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}