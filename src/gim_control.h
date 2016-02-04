//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_H
#define GIM_CONTROL_H

#include <iostream>
#include "SBGC.h"
#include "serial_port.h"
#include <vector>
#include <tuple>
#include <sys/time.h>

class gim_control 
{
    public:
        //constructor
        gim_control();

        void followPosition(std::tuple<bool, double, double> Pos);
        void absoluteAngleControl(std::vector<double> pitchYawAngles); 
        void centerGimbal();
        void parseIncomingMessages();

    private:
        std::vector<double> calcRelativePosition(std::tuple<bool,double,double> Pos);
        void relateiveAngleControl(std::vector<double> pitchYawAngles);

        serial_port oPort;
        SBGC_Parser oSbgc_parser;
        SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
        SerialCommand b;
        SBGC_cmd_realtime_data_t rt_data;
        SBGC_cmd_api_virt_ch_control_t v;

        long int now_ms;
        long int last_mov_ms;

        bool checkConnection();
        void processIncomingMessages();


        //this should be passed in from im_proc
        const int FRAME_ROWS = 480;
        const int FRAME_COLS = 680;

        int xAngleHistory = 0;
        int yAngleHistory = 0;

        struct timeval tp;
};

#endif 
