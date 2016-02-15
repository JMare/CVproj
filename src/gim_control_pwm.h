//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_PWM_H
#define GIM_CONTROL_PWM_H

#include <iostream>
#include "SBGC.h"
#include "serial_port.h"
#include <vector>
#include <tuple>
#include <chrono>
#include <cstdlib>

class gim_control_pwm
{
    public:
        //constructor
        gim_control_pwm();

        void followPosition(std::tuple<bool, double, double> Pos);
        void absoluteAngleControl(std::vector<double> pitchYawAngles); 
        void centerGimbal();

    private:
        //FUNCTIONS
        std::vector<double> calcRelativePosition(std::tuple<bool,double,double> Pos);
        void relateiveAngleControl(std::vector<double> pitchYawAngles);
        long myclock();


        //VARIABLES
        double now_ms = 0;
        double last_mov_ms = 0;
        int xAngleHistory = 0;
        int yAngleHistory = 0;

        //CONSTANTS
        
        const int PITCH_PWM_PIN = 5;
        const int YAW_PWM_PIN = 6;

        //this should be passed in from im_proc
        const int FRAME_ROWS = 480;
        const int FRAME_COLS = 680;

        //limits in degrees
        const int PITCH_UPPER_LIMIT = 30;
        const int PITCH_LOWER_LIMIT = -30;
        const int YAW_UPPER_LIMIT = 45;
        const int YAW_LOWER_LIMIT = -45;

        const int SPEED_SCALE_FACTOR = 20;

        //Interval for movement commands
        //Lower is more jerky, higher can lead to sync loss
        const double MOVEMENT_INT = 100; //unit is ms 

        struct timeval tp;
};

#endif 
