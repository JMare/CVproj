//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_H
#define GIM_CONTROL_H

#include <iostream>
#include "SBGC.h"
#include "serial_port.h"
#include <vector>
#include <tuple>
#include <chrono>
#include <cstdlib>

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
        //FUNCTIONS
        std::vector<double> calcRelativePosition(std::tuple<bool,double,double> Pos);
        void relateiveAngleControl(std::vector<double> pitchYawAngles);
        bool checkConnection();
        void processIncomingMessages();
        long myclock();

        //API objects
        serial_port oPort;
        SBGC_Parser oSbgc_parser;
        SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
        SerialCommand b;
        SBGC_cmd_realtime_data_t rt_data;
        SBGC_cmd_api_virt_ch_control_t v;

        //VARIABLES
        double now_ms = 0;
        double last_mov_ms = 0;
        int xAngleHistory = 0;
        int yAngleHistory = 0;

        //CONSTANTS
        //this should be passed in from im_proc
        const int FRAME_ROWS = 480;
        const int FRAME_COLS = 680;

        //limits in degrees
        const int PITCH_UPPER_LIMIT = 35;
        const int PITCH_LOWER_LIMIT = 65;
        const int YAW_UPPER_LIMIT = 45;
        const int YAW_LOWER_LIMIT = 45;

        const int SPEED_SCALE_FACTOR = 20;

        //Interval for movement commands
        //Lower is more jerky, higher can lead to sync loss
        const double MOVEMENT_INT = 100; //unit is ms 

        struct timeval tp;
};

#endif 
