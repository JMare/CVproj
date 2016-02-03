//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    oSbgc_parser.init(&oPort);
    
    //move gimbal to initial position
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.speedROLL = c.speedPITCH = c.speedYAW = 30 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
    
}

void gim_control::angleControl(vector<int> pitchYawAngles)
{
    
    cout << "Angle Command" << endl;
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(0));
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(1));
    SBGC_cmd_control_send(c, oSbgc_parser);
}

void gim_control::centerGimbal()
{
    cout << "Centering Gimbal" << endl;
    c.anglePITCH = 0;
    c.angleYAW = 0;
    SBGC_cmd_control_send(c, oSbgc_parser);
}
