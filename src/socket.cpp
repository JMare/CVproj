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

