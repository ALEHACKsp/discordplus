#ifndef TEST_H
#define TEST_H

#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <thread>
#include <map>



class connection;

//
typedef std::function<void (const std::string&)> read_handler_type;
typedef std::function<void (const std::string&)> write_handler_type;
class connection
{   
public:

    connection(boost::asio::io_service& io_service, 
        const std::string& addr, const std::string& port) 
        : m_addr(addr), m_port(port), m_socket(io_service)
    {
        set_read_handler([this](const std::string& m) {this->read_handler(m);});
        set_write_handler([this](const std::string& m) {this->write_handler(m);});
        connect();  
    }
    
void connect() 
{

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(m_addr), std::stoi(m_port));
    

        
        //LOG("Info", "Trying to connect: " + m_addr + ":" + m_port);
        
        m_socket.connect(endpoint);

        m_socket.async_read_some(boost::asio::buffer(m_buffer), 
            boost::bind(&connection::read, this,
                boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred
            )
        );
        

    
    
    //LOG("Info", "Connected!");
}

    void close()
    {
        m_socket.close();
       // m_io_service.stop();
    }


void write(const std::string& content)
{
    //LOG("Write", content);
    boost::asio::write(m_socket, boost::asio::buffer(content + "\r\n"));
}

void read(const boost::system::error_code& error, std::size_t count)
{
    if (error) {
        close();
    }
    else {
        m_read_handler(std::string(m_buffer.data(), count));
        
        m_socket.async_read_some(boost::asio::buffer(m_buffer), 
            boost::bind(&connection::read, this,
                boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred
            )
        ); /*
        boost::asio::async_read(m_socket, boost::asio::buffer(m_buffer), boost::asio::transfer_at_least(1),
        boost::bind(&connection::read, this,
                    boost::asio::placeholders::error, 
                    boost::asio::placeholders::bytes_transferred)); */
    }
}
void read_handler(const std::string& message)
{
    for (auto func: m_read_handlers) {
        func(message);
    }
}

void write_handler(const std::string& msg)
{  
    write(msg);
}


void set_read_handler(const read_handler_type& handler)
{
    m_read_handler = handler;
}

void set_write_handler(const write_handler_type& handler)
{
    m_write_handler = handler;
}

void add_read_handler(std::function<void (const std::string&)> func)
{
    m_read_handlers.push_back(func);
}
    
private:



    std::string m_addr;
    std::string m_port;
    
    boost::asio::ip::tcp::socket m_socket;
    
    std::array<char, 256> m_buffer;

    read_handler_type m_read_handler;
    write_handler_type m_write_handler;
    std::vector<std::function<void (const std::string&)>> m_read_handlers;
};


#endif