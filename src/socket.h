#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

class socket_handler
{
    public:
        socket_handler();
        ~socket_handler();
        void socket_init();

        void run();
        boost::thread    m_Thread;
        boost::asio::io_service io_service;
    private:
        

};

#endif
