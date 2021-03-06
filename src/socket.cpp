#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <string>
#include "cvproj.h"
#include "socket.h"
#include <pthread.h>
#include <thread>
#include "laserlink.h"
#include "params.h"

int count = 0;

using boost::asio::ip::tcp;

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  int id;

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
    //Socket opened
    
    id = count;
    count++;
    std::cout << "Socket Opened # "<< id << std::endl;
    //start the async header read
    //callback to handle_read_header
    boost::asio::async_read_until(socket_, response_,"EHX",
        boost::bind(&tcp_connection::handle_read_header, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  }

private:
  tcp_connection(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {
  }

  void handle_read_header(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if ((boost::asio::error::eof == error) ||
        (boost::asio::error::connection_reset == error))
    {
        // handle the disconnect.
        std::cout << "Disconnect handled # " << id << std::endl;
    }
    else
    {
        //Handle the data
        //convert buff to a string
        std::istream is(&response_);
        std::string line;
        std::getline(is, line);

        std::cout << "Header received: " << line << std::endl;

        laserEnum messageEnum;
        int bodylen = parse_header(line, &messageEnum);

        std::cout << "body length: " << bodylen << std::endl;

        switch(messageEnum){
           case PRQ: //Parameter request
               std::cout << "Parameter request received" << std::endl;
               std::string MessageBody = gParams.packForTelemetry();
               std::string messHead = "SHX087PRPEHX"; //Parameter reply
               std::string bodyBeg = "SMX";
               std::string bodyEnd = "EMX";
               std::string messTot = messHead + bodyBeg + MessageBody + bodyEnd;
                boost::asio::async_write(socket_, boost::asio::buffer(messTot),
                    boost::bind(&tcp_connection::handle_write, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
                break;
        }
        
        if(bodylen > 0){
            //Message has body, start async read until end of message
            //callback to handle_read_body 
            boost::asio::async_read_until(socket_, response_,"EMX",
                boost::bind(&tcp_connection::handle_read_body, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                messageEnum, //what type of body is it
                bodylen)); //how long is the body (not including markers
        } else{
            //Message has no body, start async read  
            //callback to handle_read_body 
            boost::asio::async_read_until(socket_, response_,"EHX",
                boost::bind(&tcp_connection::handle_read_header, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }
  }

  void handle_read_body(const boost::system::error_code& error,
      size_t bytes_transferred,
      laserEnum type,
      int len)
  {
    if ((boost::asio::error::eof == error) ||
        (boost::asio::error::connection_reset == error))
    {
        // handle the disconnect.
        std::cout << "Disconnect handled # " << id << std::endl;
    }
    else
    {
        //Handle the data
        //convert buff to a string
        std::istream is(&response_);
        std::string line;
        std::getline(is, line);
        //print message
        std::cout << "Body received: " << line << std::endl;
        
        //switch on enum
        switch(type){
           case DOC:
               {
                ll_do commandIn(line, len);
                commandIn.execute_command(); 
                break;
               }
            case MOV:
               {
                ll_mov commandIn(line,len);
                commandIn.execute_command();
                break;
               }
        }
            

        //start reading header 
        boost::asio::async_read_until(socket_, response_,"EHX",
            boost::bind(&tcp_connection::handle_read_header, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

  tcp::socket socket_;
  std::string message_;
  boost::asio::streambuf response_;
};

class tcp_server
{
public:
  tcp_server(boost::asio::io_service& io_service)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
  {
    start_accept();
  }

private:
  int testint = 0;
  void start_accept()
  {
    tcp_connection::pointer new_connection =
      tcp_connection::create(acceptor_.get_io_service());

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_connection->start();
      start_accept();
    }
  }

  tcp::acceptor acceptor_;
};


socket_handler::socket_handler()
{
  //constructor for socket class
  
    //boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
}

void socket_handler::socket_init()
{
    //run() will block for the entire execution of the program

    m_Thread = boost::thread(&socket_handler::run, this);
    //boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

}

void socket_handler::run()
{
    tcp_server server(io_service);
    io_service.run();
}

socket_handler::~socket_handler()
{
    io_service.stop();
    m_Thread.join();
}

