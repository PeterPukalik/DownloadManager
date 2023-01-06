#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include "../header/Data.h"
#include "../header/Ftp.h"

using boost::asio::ip::tcp;

int ftp(Data* data)
{
    try
    {
//        if (argc != 3)
//        {
//            std::cerr << "Usage: ftp_client <host> <path>" << std::endl;
//            return 1;
//        }
//
//        boost::asio::io_service io_service;
//        // Create a socket and connect it to the FTP server.
//        boost::asio::io_context io_context;
//        tcp::socket socket(io_context);
//        tcp::resolver resolver(io_service);
//        boost::asio::ip::tcp::resolver::query query("ftp.gnu.org", "ftp");
//
//        boost::asio::connect(socket, resolver.resolve(query));
//
//        // Send the FTP "retr" command to download the file.
//        boost::asio::streambuf request;
//        std::ostream request_stream(&request);
//        request_stream << "USER anonymous\r\n";
//        request_stream << "retr " << "/third-party/README" << "\r\n";
//
//        boost::asio::write(socket, request);
//
//        // Read the FTP response.
//        boost::asio::streambuf response;
//        boost::asio::read_until(socket, response, "\r\n");
//
//        // Check the response status code.
//        std::istream response_stream(&response);
//        std::string line;
//        std::getline(response_stream, line);
//        boost::system::error_code error;
//        if (!line.empty() && line[0] == '2')
//        {
//            // Open the output file.
//            std::ofstream output_file;
//            output_file.open("/third-party/README", std::ios::out | std::ios::binary);
//
//            // Read the file from the server and write it to the output file.
//            boost::asio::read(socket, response,
//                              boost::asio::transfer_at_least(1), error);
//            while (response.size() > 0)
//            {
//                output_file << &response;
//                boost::asio::read(socket, response,
//                                  boost::asio::transfer_at_least(1), error);
//            }
//
//            output_file.close();
//        }
//        else
//        {
//            std::cerr << "Error: " << line << std::endl;
//        }
        boost::asio::io_context io_context;

        // Create a socket and connect to the FTP server.
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve("ftp.gnu.org", "21"));

        // Send the FTP USER command to log in.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "USER anonymous\r\n";
        boost::asio::write(socket, request);

        // Receive the FTP server's response.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");
        std::istream response_stream(&response);
        std::string response_string;
        std::getline(response_stream, response_string);
        std::cout << "FTP server response: " << response_string << "\n";

        // Send the FTP PASS command to log in.
        request.consume(request.size());
        request_stream.clear();
        request_stream << "PASS \r\n";
        boost::asio::write(socket, request);

        // Receive the FTP server's response.
        response.consume(response.size());
        boost::asio::read_until(socket, response, "\r\n");
        std::getline(response_stream, response_string);
        std::cout << "FTP server response: " << response_string << "\n";

        // Get the IP address and port number of the client.
        boost::asio::ip::tcp::endpoint local_endpoint = socket.local_endpoint();
        std::string ip_address = local_endpoint.address().to_string();
        std::string port = std::to_string(local_endpoint.port());

        // Send the FTP PORT command to specify the data transfer mode and connection parameters.
        request.consume(request.size());
        request_stream.clear();
        request_stream << "PORT " << ip_address << "," << port[0] << "," << port[1] << "\r\n";
        boost::asio::write(socket, request);

        // Send the FTP RETR command to retrieve a file.
        request.consume(request.size());
        request_stream.clear();
        request_stream << "RETR /third-party/README.txt\r\n";
        boost::asio::write(socket, request);

        std::ofstream output_file;
        output_file.open("text.txt",  std::ios::binary);

        // Read the file from the server and write it to the output file.
        boost::system::error_code error;
        boost::asio::read(socket, response,
                          boost::asio::transfer_at_least(1), error);
        while (response.size() > 0)
        {
            std::cout << &response;
            boost::asio::read(socket, response,
                              boost::asio::transfer_at_least(1), error);
        }

        output_file.close();

        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}