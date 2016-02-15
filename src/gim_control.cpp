//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
absoluteAngleControl({0,0});
}

void gim_control::followPosition(tuple<bool, double, double> Pos)
{
    now_ms = myclock();

    if(now_ms - last_mov_ms >  MOVEMENT_INT){  
        last_mov_ms = myclock();

        if(get<0>(Pos))
        {
            vector<double> relPos = calcRelativePosition(Pos);
            relateiveAngleControl(relPos);
        } /*else 
        { 
            centerGimbal();
            xAngleHistory = 0;
            yAngleHistory = 0;
        }*/
    }
}

vector<double> gim_control::calcRelativePosition(tuple<bool, double, double> Pos)
{
    double x = get<1>(Pos);
    double y = get<2>(Pos);

    //These scale the angle change linearly.
    const double GAIN_X = 0.03;
    const double GAIN_Y = 0.03;

    //These calculate a score between -100 and 100 for x and y
    double xCorrected = ((x - (FRAME_COLS / 2)) / (FRAME_COLS / 2)) * 100;
    double yCorrected = ((y - (FRAME_ROWS / 2)) / (FRAME_ROWS / 2)) * 100;
   
    double xRelativeAngle, yRelativeAngle;

    if(xCorrected > 20 || xCorrected < -20){ 
        xRelativeAngle = GAIN_X * xCorrected;
    } else xRelativeAngle = 0;

    if(yCorrected > 20 || yCorrected < -20){ 
        yRelativeAngle = GAIN_Y * yCorrected;
    } else  yRelativeAngle = 0;

    return {yRelativeAngle, xRelativeAngle};
}

void gim_control::relateiveAngleControl(vector<double> pitchYawAngles)
{
   double xAngleCmd = pitchYawAngles.at(1) + xAngleHistory;
   double yAngleCmd = pitchYawAngles.at(0) + yAngleHistory;

   if(xAngleCmd < YAW_LOWER_LIMIT) xAngleCmd = YAW_LOWER_LIMIT;
   if(xAngleCmd > YAW_UPPER_LIMIT) xAngleCmd = YAW_UPPER_LIMIT;
   if(yAngleCmd < PITCH_LOWER_LIMIT) yAngleCmd = PITCH_LOWER_LIMIT;
   if(yAngleCmd > PITCH_UPPER_LIMIT) yAngleCmd = PITCH_UPPER_LIMIT;

   absoluteAngleControl({yAngleCmd, xAngleCmd});

   xAngleHistory = xAngleCmd;
   yAngleHistory = yAngleCmd;
}

void gim_control::absoluteAngleControl(vector<double> pitchYawAngles)
{
    //turn angle into pwm % given known limits
    int pitchPwm = ((pitchYawAngles.at(0) - PITCH_LOWER_LIMIT) / (PITCH_UPPER_LIMIT - PITCH_LOWER_LIMIT)) * 100; 
    string pitchPwmCmd = "echo ";
    pitchPwmCmd += to_string(PITCH_PWM_PIN);
    pitchPwmCmd += "=";
    pitchPwmCmd += to_string(pitchPwm);
    pitchPwmCmd += "%";
    pitchPwmCmd += " > /dev/servoblaster";
    system(pitchPwmCmd.c_str());

    int yawPwm = ((pitchYawAngles.at(1) - YAW_LOWER_LIMIT) / (YAW_UPPER_LIMIT - YAW_LOWER_LIMIT)) * 100; 
    string yawPwmCmd = "echo ";
    yawPwmCmd += to_string(YAW_PWM_PIN);
    yawPwmCmd += "=";
    yawPwmCmd += to_string(yawPwm);
    yawPwmCmd += "%";
    yawPwmCmd += " > /dev/servoblaster";
    system(yawPwmCmd.c_str());
}


void gim_control::centerGimbal()
{
}

bool gim_control::checkConnection()
{
}

void gim_control::processIncomingMessages()
{
}

long gim_control::myclock()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
}
