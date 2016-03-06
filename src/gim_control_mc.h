//Written by James Mare
//Handles serial control of the alexmos gimbal

#ifndef GIM_CONTROL_MC_H
#define GIM_CONTROL_MC_H

#include <iostream>
#include <vector>
#include <tuple>
#include <chrono>
#include <cstdlib>
#include "im_proc.h"
#include "params.h"
#include "cvproj.h"
#include "serialstream.h"
#include "SimpleSerial.h"
#include "TimeoutSerial.h"
#include "BufferedAsyncSerial.h"

class gim_control_mc
{
    public:
        //constructor
        gim_control_mc();

        void followPosition(laserInfo Pos);
        void absoluteAngleControl(std::vector<double> pitchYawAngles); 
        void centerGimbal();

    private:
        //FUNCTIONS
        std::vector<double> calcRelativePosition(laserInfo Pos);
        void relateiveAngleControl(std::vector<double> pitchYawAngles);
        void writeRCSignals(int pitchPwm, int yawPwm);
        void readRCSignals();
        long myclock();

        //VARIABLES
        double now_ms = 0;
        double last_mov_ms = 0;
        int xAngleHistory = 0;
        int yAngleHistory = 0;

        int YawPwm, PitchPwm, EnPwm;

        //CONSTANTS
        
        //rpi pins for pwm
        const int PITCH_PWM_PIN = 5;
        const int YAW_PWM_PIN = 6;

        //this should be passed in from im_proc
        const int FRAME_ROWS = 480;
        const int FRAME_COLS = 680;

        //limits in degrees
        const int PITCH_UPPER_LIMIT = 25;
        const int PITCH_LOWER_LIMIT = -25;
        const int YAW_UPPER_LIMIT = 60;
        const int YAW_LOWER_LIMIT = -60;

        struct timeval tp;

};

#endif 
