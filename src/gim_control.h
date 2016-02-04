//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_H
#define GIM_CONTROL_H

#include <iostream>
#include "SBGC.h"
#include "serial_port.h"
#include <vector>
#include <tuple>

class gim_control 
{
    public:
        //constructor
        gim_control();

        void followPosition(std::tuple<bool, double, double> Pos);
        void absoluteAngleControl(std::vector<int> pitchYawAngles); 
        void centerGimbal();
        void parseIncomingMessages();

    private:
        std::vector<int> calcRelativePosition(std::tuple<bool,double,double> Pos);
        void relateiveAngleControl(std::vector<int> pitchYawAngles);

        serial_port oPort;
        SBGC_Parser oSbgc_parser;
        SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
        SerialCommand b;
        SBGC_cmd_realtime_data_t rt_data;

        time_t now;
        time_t time_last_movement = 0;

        bool checkConnection();
        void processIncomingMessages();


        //this should be passed in from im_proc
        const int FRAME_ROWS = 480;
        const int FRAME_COLS = 680;

        int xAngleHistory = 0;
        int yAngleHistory = 0;
};

#endif 
