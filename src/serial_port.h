//Written by James Mare
//Class definiton to handle low level serial port stuff

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <iostream>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "SBGC.h"

class serial_port: public SBGC_ComObj
{
    public:
        serial_port();
        
        void writeByte(uint8_t b);
        uint8_t readByte();
        uint16_t getBytesAvailable();
        uint16_t getOutEmptySpace(){
            return 0xFFFF;
        }

    private:
        int set_interface_attribs(int fd, int speed, int parity);
        void set_blocking (int fd, int should_block);
        int fd;
};

#endif
