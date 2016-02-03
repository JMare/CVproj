//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_H
#define GIM_CONTROL_H

#include <iostream>
#include "SBGC.h"
#include "serial_port.h"
#include <vector>

class gim_control 
{
    public:
        //constructor
        gim_control();
        void angleControl(std::vector<int> pitchYawAngles); 
        void centerGimbal();
        void parseIncomingMessages();

    private:
        serial_port oPort;
        SBGC_Parser oSbgc_parser;
        SBGC_cmd_control_t c;
        SBGC_cmd_control_ext_t cmd_control = { 
            {SBGC_CONTROL_MODE_ANGLE, SBGC_CONTROL_MODE_RC, SBGC_CONTROL_MODE_RC} ,
            { { 0, 0 }, { 0, 0 }, { 0, 0 } }  // angle and speed
        }; 
};

#endif 
