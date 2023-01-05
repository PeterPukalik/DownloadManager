//
// Created by puki on 4.1.2023.
//


#include "../header/Https.h"

//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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
//
//class client
//{
//public:
//    client(boost::asio::io_context& io_context,
//           boost::asio::ssl::context& context,
//           boost::asio::ip::tcp::resolver::results_type endpoints)
//            : socket_(io_context, context)
//    {
//        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
//        socket_.set_verify_callback(
//                boost::bind(&client::verify_certificate, this,
//                            boost::placeholders::_1, boost::placeholders::_2));
//
//        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
//                                   boost::bind(&client::handle_connect, this,
//                                               boost::asio::placeholders::error));
//    }
//
//    bool verify_certificate(bool preverified,
//                            boost::asio::ssl::verify_context& ctx)
//    {
//        // The verify callback can be used to check whether the certificate that is
//        // being presented is valid for the peer. For example, RFC 2818 describes
//        // the steps involved in doing this for HTTPS. Consult the OpenSSL
//        // documentation for more details. Note that the callback is called once
//        // for each certificate in the certificate chain, starting from the root
//        // certificate authority.
//
//        // In this example we will simply print the certificate's subject name.
//        char subject_name[256];
//        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
//        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
//        std::cout << "Verifying " << subject_name << "\n";
//
//        return preverified;
//    }
//
//    void handle_connect(const boost::system::error_code& error)
//    {
//        if (!error)
//        {
//            socket_.async_handshake(boost::asio::ssl::stream_base::client,
//                                    boost::bind(&client::handle_handshake, this,
//                                                boost::asio::placeholders::error));
//        }
//        else
//        {
//            std::cout << "Connect failed: " << error.message() << "\n";
//        }
//    }
//
//    void handle_handshake(const boost::system::error_code& error)
//    {
//        if (!error)
//        {
//            std::cout << "Enter message: ";
//            std::cin.getline(request_, max_length);
//            size_t request_length = strlen(request_);
//
//            boost::asio::async_write(socket_,
//                                     boost::asio::buffer(request_, request_length),
//                                     boost::bind(&client::handle_write, this,
//                                                 boost::asio::placeholders::error,
//                                                 boost::asio::placeholders::bytes_transferred));
//        }
//        else
//        {
//            std::cout << "Handshake failed: " << error.message() << "\n";
//        }
//    }
//
//    void handle_write(const boost::system::error_code& error,
//                      size_t bytes_transferred)
//    {
//        if (!error)
//        {
//            boost::asio::async_read(socket_,
//                                    boost::asio::buffer(reply_, bytes_transferred),
//                                    boost::bind(&client::handle_read, this,
//                                                boost::asio::placeholders::error,
//                                                boost::asio::placeholders::bytes_transferred));
//        }
//        else
//        {
//            std::cout << "Write failed: " << error.message() << "\n";
//        }
//    }
//
//    void handle_read(const boost::system::error_code& error,
//                     size_t bytes_transferred)
//    {
//        if (!error)
//        {
//            std::cout << "Reply: ";
//            std::cout.write(reply_, bytes_transferred);
//            std::cout << "\n";
//        }
//        else
//        {
//            std::cout << "Read failed: " << error.message() << "\n";
//        }
//    }
//
//private:
//    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
//    char request_[max_length];
//    char reply_[max_length];
//};
//
//
//
//
//


int https(Data *data) {

    try
    {
        //if (argc != 3)
        //{
            //std::cerr << "Usage: client <host> <port>\n";
            //return 1;
        //}
//
//        boost::asio::io_context io_context;
//
//        boost::asio::ip::tcp::resolver resolver(io_context);
//        boost::asio::ip::tcp::resolver::results_type endpoints =
//                resolver.resolve("www.google.com", "443");
//
//        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
//        ctx.set_default_verify_paths();
//        //ctx.load_verify_file("CA_Disig_Root_R2.pem");
//
//        client c(io_context, ctx, endpoints);
//
//        io_context.run();

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
            std::cout << header << "\n";
        }
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
            data->addAllreadyDownloaded(response.size());
            //*allreadyDownloaded += response.size();
            outdata << &response;
        }

        // Read until EOF, writing Data to output as we go.
        boost::system::error_code error;
        while (boost::asio::read(sock , response,boost::asio::transfer_at_least(1), error) && !data->isStop()) {
            data->addAllreadyDownloaded(response.size());
            //*allreadyDownloaded += response.size();
            outdata << &response;
        }
        outdata.close();
        if(data->isStop()){
            data->setFlag(2);//stoped
        }else{
            data->setFlag(3);//finished
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
