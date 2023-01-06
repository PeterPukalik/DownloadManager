//
// Created by puki on 4.1.2023.
//


#include "../header/Https.h"



#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

enum { max_length = 1024 };
using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;


int https(Data *data) {

    try
    {

// Create a context that uses the default paths for
// finding CA certificates.
        ssl::context ctx(ssl::context::tls);//podmienka tiez na tls
        ctx.set_default_verify_paths();

// Open a socket and connect it to the remote host.
        boost::asio::io_context io_context;
        ssl_socket sock(io_context, ctx);
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(data->getWeb(), "https");
        boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));
        sock.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
        sock.set_verify_mode(ssl::verify_peer);
        sock.set_verify_callback(ssl::rfc2818_verification(data->getWeb()));

        //doplny hostname pri hanshake SSL_set_tlsext_host_name(sock.native_handle(),host.c_str())
        if(!SSL_set_tlsext_host_name(sock.native_handle(),data->getWeb().c_str())){
            std::cout << "random \n";
            throw boost::system::system_error(::ERR_get_error(),boost::asio::error::get_ssl_category());
        }
        sock.handshake(ssl_socket::client);

        int responselength = 0;
        // Get a list of endpoints corresponding to the server name.

        tcp::resolver::results_type endpoints = resolver.resolve(data->getWeb(), "https");

        // Try each endpoint until we successfully establish a connection.

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
        boost::asio::write(sock, request);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(sock, response, "\r\n");

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
        //TODO odkomentovat
        //if (status_code != 200 && status_code != 206 && status_code != 301 )
        //{
            std::cout << "Response returned with status code " << status_code << "\n";
            //return 1;
        //}

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(sock, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        data->setFlag(1);//started
        while (std::getline(response_stream, header) && header != "\r"){
            if(header.compare(0,16,"Content-Length: ")== 0){
                data->setTotalSize(std::stoi(header.substr(16,header.length())));
            }
            //std::cout << header << "\n";
            //
        }
        std::cout << "zacal som stahovat" << "\n";
        //std::cout << "\n";

        // Write whatever content we already have to output.
        std::ofstream outdata;
        outdata.open((data->getName() + ".dat"), std::ios::out);
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
        while (boost::asio::read(sock , response,boost::asio::transfer_at_least(1), error) && !data->isStop()) {
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
        std::cerr << "Exception: " << e.what() << "\n";
    }


    return 0;

}
