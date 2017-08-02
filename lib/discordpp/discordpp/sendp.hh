#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

namespace discordpp{
    class sendpack{
    public:
        sendpack(std::string host, int port):host_(host),port_(port){}
        void send(std::string message)
        {
    boost::asio::io_service ios;
            
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_);

        boost::asio::ip::tcp::socket socket(ios);

    socket.connect(endpoint);

    boost::array<char, 128> buf;
        std::copy(message.begin(),message.end(),buf.begin());
    boost::system::error_code error;
    socket.write_some(boost::asio::buffer(buf, message.size()), error);
        socket.close();
        std::cout << message+"\n";
    }

private:
    std::string host_ = "";
    int port_;
};


}