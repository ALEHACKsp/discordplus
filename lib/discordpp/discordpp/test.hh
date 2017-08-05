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
        const std::string& addr, const std::string& port, 
        const std::string& accname, const std::string& password) 
        : m_addr(addr), m_port(port), m_socket(io_service), m_resolver(io_service),
        m_accname(accname), m_password(password)
    {
        set_read_handler([this](const std::string& m) {this->read_handler(m);});
        set_write_handler([this](const std::string& m) {this->write_handler(m);});
        m_resolver.async_resolve(
        boost::asio::ip::tcp::resolver::query(m_addr, m_port),
        boost::bind(&connection::handle_resolve, this, _1, _2));  
    }

    void reconnect()
    {
        m_resolver.async_resolve(
        boost::asio::ip::tcp::resolver::query(m_addr, m_port),
        boost::bind(&connection::handle_resolve, this, _1, _2));  
    }
    
    void handle_resolve(const boost::system::error_code& err,
                         tcp::resolver::iterator endpoint_iterator)
    {
    if (!err) {
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        m_socket.async_connect(
            endpoint,
            boost::bind(&connection::handle_connect, this, _1, ++endpoint_iterator));
    } else {
        std::cerr << "Error1: " << err.message() << std::endl;
    }
    }

void handle_connect(const boost::system::error_code& err,
                         tcp::resolver::iterator endpoint_iterator)
{
    if (!err) {
        connected = true;
        boost::asio::async_read_until(m_socket, m_buffer, "\r\n", 
        boost::bind(&connection::sign_in, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
    } else if (endpoint_iterator != tcp::resolver::iterator()) {
        m_socket.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        m_socket.async_connect(
            endpoint,
            boost::bind(&connection::handle_connect, this, _1, ++endpoint_iterator));
    } else {
        std::cerr << "Error2: " << err.message() << std::endl;
        reconnect();
    }
}  

void sign_in(const boost::system::error_code& err, std::size_t count)
{
    if (!err) {
        std::string message;
        std::getline(std::istream(&m_buffer), message);

        if(message.find("Username: ") != -1)
        {
            this->write(m_accname);
            boost::asio::async_read_until(m_socket, m_buffer, "Password: ",
            boost::bind(&connection::sign_in, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
        }
        else if(message.find("Password: ") != -1)
        {
            this->write(m_password);
            boost::asio::async_read_until(m_socket, m_buffer, "\r\n",
            boost::bind(&connection::read, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
            std::cout << "Signed in to TC\n";
        }
        else
        {

            boost::asio::async_read_until(m_socket, m_buffer, "Username: ",
            boost::bind(&connection::sign_in, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
        }
    } else {
        std::cerr << "Error3: " << err.message() << std::endl;
    }
}  


    void close()
    {
        m_socket.close();
        std::cout << "TC Connection Closed\n";
       // m_io_service.stop();
        connected = false;
        this->reconnect();
        std::cout << "TC Reconnecting\n";
    }


void write(const std::string& content)
{
    //LOG("Write", content);
    if(connected)
        boost::asio::write(m_socket, boost::asio::buffer(content + "\r\n"));
}

void read(const boost::system::error_code& error, std::size_t count)
{
    if (error) {
        close();
    }
    else {
        std::string s( (std::istreambuf_iterator<char>(&m_buffer)), std::istreambuf_iterator<char>() );
        std::string message;
        //std::getline(std::istream(&m_buffer), message);
              //  std::cout << message+"\n";
        if(s.find("Authentication Required")== 0)
        {
            boost::asio::async_read_until(m_socket, m_buffer, "Username: ",
            boost::bind(&connection::sign_in, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
        }
        else
        {
        m_read_handler(s);
        /*
        m_socket.async_read_some(m_buffer, 
            boost::bind(&connection::read, this,
                boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred
            )
        ); */
        boost::asio::async_read_until(m_socket, m_buffer, "\r\n",
        boost::bind(&connection::read, this,boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred)); 
        }
        /*
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
    std::string m_accname;
    std::string m_password;

    bool connected = false;
    
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;
    
    boost::asio::streambuf m_buffer;

    read_handler_type m_read_handler;
    write_handler_type m_write_handler;
    std::vector<std::function<void (const std::string&)>> m_read_handlers;
};


#endif