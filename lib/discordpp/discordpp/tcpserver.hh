//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef TCPSERVER_HH
#define TCPSERVER_HH

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
//#include <lib/nlohmannjson/src/json.hpp>
#include <nlohmann/json.hpp>
#include "tcpconn.hh"


using boost::asio::ip::tcp;
class tcp_server;

//
using Handler2 = std::function<void(tcp_server*,  std::string)>;

class tcp_server
{
public:

  tcp_server(boost::asio::io_service& io_service, int port)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

  void start_accept()
  {
    tcp_connection::pointer new_connection =
      tcp_connection::create(acceptor_.get_io_service());

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }
  void gotmessage(std::string msg)
  {
    //std::cout << msg +"\n";
    handleDispatch("MESSAGE_CREATE", msg);
  }

  //
  void addHandler(std::string event, Handler2 handler)
  {
    handlers_.insert(std::pair<std::string, Handler2>(event, handler));
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


  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_connection->addHandler("MESSAGE_CREATE", [this](tcp_connection* bot, std::string msg){
            this->gotmessage(msg);
        });
      new_connection->start();
      start_accept();
    }
  }

  tcp::acceptor acceptor_;
  std::multimap<std::string, Handler2> handlers_ = {};
};

#endif