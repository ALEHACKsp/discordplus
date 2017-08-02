#ifndef TCPCONN_HH
#define TCPCONN_HH

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
//#include <lib/nlohmannjson/src/json.hpp>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;

std::string make_string(boost::asio::streambuf& streambuf)
{
 return {buffers_begin(streambuf.data()), 
         buffers_end(streambuf.data())};
}

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}
//using Handler = std::function<void(tcp_connection*, aios_ptr, json)>;
class tcp_connection;
using Handler = std::function<void(tcp_connection*, std::string)>;
class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{

    boost::array<char, 30> buf;
public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_service& io_service)
  {
    return pointer(new tcp_connection(io_service));
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
   // message_ = make_daytime_string();
/*
    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)); */



     // std::cout << message_;
    //start_read();

        boost::asio::async_read_until(socket_, input_buffer_, "\r\n",
        boost::bind(&tcp_connection::start_read, shared_from_this(),boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 

            
  }

  void addHandler(std::string event, Handler handler)
  {
    handlers_.insert(std::pair<std::string, Handler>(event, handler));
  }

private:
        void handleDispatch(std::string event, std::string msg) {
            //std::cout << event << '\n';
            for(auto handler : handlers_){
                //std::cout << "    " << handler.first   << '\n';
                if(handler.first == event){
                    handler.second(this, msg);
                }
            }
          }


  tcp_connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

    void start_read(const boost::system::error_code& error, std::size_t count)
  {
    handleDispatch("MESSAGE_CREATE", make_string(input_buffer_));
    // Start an asynchronous operation to read a newline-delimited message.

         start();
  }

void handle_read(const boost::system::error_code& error)
{
    if(!error)
    {
        handleDispatch("MESSAGE_CREATE", make_string(input_buffer_));
        //std::cout << "Message: " << make_string(input_buffer_) << std::endl;
        //start_read();
    }
    else
    {
        std::cout << "Error occurred." << error.message() << std::endl;
    }
}


  tcp::socket socket_;
  std::string message_;
  boost::asio::streambuf input_buffer_;
  std::multimap<std::string, Handler> handlers_ = {};
};
#endif