//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    serial_port oPort;
    SBGC_Parser oSbgc_parser;
    SerialCommand b;

    oSbgc_parser.init(&oPort);


    //move gimbal to initial position
    SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.speedROLL = c.speedPITCH = c.speedYAW = 30 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
    usleep(5000000);

    
    cout << "sent command 1" << endl;

    usleep(5000000);
}

void gim_control::angleControl(vector<int> pitchYawAngles)
{
    SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };

    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(0));
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(1));
    SBGC_cmd_control_send(c, oSbgc_parser);
}
