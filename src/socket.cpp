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
#include "socket.h"
#include <pthread.h>
#include <thread>


using boost::asio::ip::tcp;

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{
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

  void start(int testint)
  {
    
    message_ = "this is a test string " + std::to_string(testint);

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
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

  tcp::socket socket_;
  std::string message_;
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
      new_connection->start(testint);
      testint++;
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

