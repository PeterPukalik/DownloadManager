#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include "../header/Data.h"

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

        boost::asio::io_service io_service;
        // Create a socket and connect it to the FTP server.
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query("ftp.gnu.org", "ftp");

        boost::asio::connect(socket, resolver.resolve(query));

        // Send the FTP "retr" command to download the file.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "retr " << "/third-party/README" << "\r\n";
        boost::asio::write(socket, request);

        // Read the FTP response.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check the response status code.
        std::istream response_stream(&response);
        std::string line;
        std::getline(response_stream, line);
        boost::system::error_code error;
        if (!line.empty() && line[0] == '2')
        {
            // Open the output file.
            std::ofstream output_file;
            output_file.open("/third-party/README", std::ios::out | std::ios::binary);

            // Read the file from the server and write it to the output file.
            boost::asio::read(socket, response,
                              boost::asio::transfer_at_least(1), error);
            while (response.size() > 0)
            {
                output_file << &response;
                boost::asio::read(socket, response,
                                  boost::asio::transfer_at_least(1), error);
            }

            output_file.close();
        }
        else
        {
            std::cerr << "Error: " << line << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}