#include "../header/Data.h"
#include "../header/Ftp.h"
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <vector>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>
#include <boost/asio/ssl/stream.hpp>




using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

int ftps(Data* data) {
    try
    {

        // Create a context that uses the default paths for
// finding CA certificates.
    ssl::context ctx(ssl::context::tls);//podmienka tiez na tls
    ctx.set_default_verify_paths();

// Open a socket and connect it to the remote host.
    boost::asio::io_context io_context;
    ssl_socket socket(io_context, ctx);
    tcp::resolver resolver(io_context);
    tcp::resolver::query query(data->getWeb(), "ftps");
    boost::asio::connect(socket.lowest_layer(), resolver.resolve(query));
    socket.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
    socket.set_verify_mode(ssl::verify_peer);
    socket.set_verify_callback(ssl::rfc2818_verification(data->getWeb()));

    //doplny hostname pri hanshake SSL_set_tlsext_host_name(sock.native_handle(),host.c_str())
    if(!SSL_set_tlsext_host_name(socket.native_handle(),data->getWeb().c_str())){
        std::cout << "random \n";
        throw boost::system::system_error(::ERR_get_error(),boost::asio::error::get_ssl_category());
    }
    socket.handshake(ssl_socket::client);


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
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "USER1 FTP server response: " << response_string << "\n";

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
    boost::asio::ip::tcp::endpoint local_endpoint = socket.lowest_layer().local_endpoint();
    std::string ip_address = local_endpoint.address().to_string();
    std::string port = std::to_string(local_endpoint.port());

    std::cout << "IP FTP server response: " << response_string << "\n";

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
        if(data->getTotalSize() == 0) {
            pthread_mutex_lock(data->getMutex());
            data->setTotalSize(stoi(file_size));
            pthread_mutex_unlock(data->getMutex());
        }
    } else {
        std::cerr << "Error: invalid response from FTP server\n";
    }
    request.consume(request.size());
    request_stream.clear();
    request_stream << "TYPE I" << "\r\n";
    boost::asio::write(socket, request);
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "TYPE FTP server response: " << response_string << "\n";
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
//    boost::asio::ip::tcp::socket data_socket(io_context);
//    boost::asio::ip::tcp::endpoint data_endpoint(boost::asio::ip::make_address(data_ip_address), data_port);
//    data_socket.connect(data_endpoint);



    // Send the FTP RETR command to retrieve a file.
    request.consume(request.size());
    request_stream.clear();
    request_stream << "RETR " << data->getPath()<<"\r\n";
    boost::asio::write(socket, request);
    // Open a file to write the data to.
    boost::asio::read_until(socket, response, "\r\n");
    std::getline(response_stream, response_string);
    std::cout << "RETR FTP server response: " << response_string << "\n";
    std::ofstream output_file(data->getName() +".dat", std::ios::binary | std::ios_base::app);
    if (output_file.fail()) {
        std::cerr << "Error opening file\n";
        return 1;
    }
    ssl::context ctx1(ssl::context::tls);//podmienka tiez na tls
    ctx1.set_default_verify_paths();

// Open a socket and connect it to the remote host.
    boost::asio::io_context io_context1;
    ssl_socket socket1(io_context1, ctx1);
    tcp::resolver resolver1(io_context1);
    tcp::resolver::query query1(data_ip_address, std::to_string(data_port));
    boost::asio::connect(socket1.lowest_layer(), resolver1.resolve(query1));
    socket1.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
    socket1.set_verify_mode(ssl::verify_peer);
    socket1.set_verify_callback(ssl::rfc2818_verification(data->getWeb()));

    //doplny hostname pri hanshake SSL_set_tlsext_host_name(sock.native_handle(),host.c_str())
    if(!SSL_set_tlsext_host_name(socket1.native_handle(),data_ip_address.c_str())){
        std::cout << "random \n";
        throw boost::system::system_error(::ERR_get_error(),boost::asio::error::get_ssl_category());
    }
    socket1.handshake(ssl_socket::client);
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
    while (boost::asio::read(socket1, response, boost::asio::transfer_at_least(1), error)  && !data->isStop())
    {
        pthread_mutex_lock(data->getMutex());
        data->addAllreadyDownloaded(response.size());
        pthread_mutex_unlock(data->getMutex());
        output_file << &response;
    }

    output_file.close();
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
    //std::cerr << responseSizeout << std::endl;
//    if (error == boost::asio::error::eof) {
//        error.clear();
//    } else if (error) {
//        throw boost::system::system_error(error);
//    }

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