//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    oSbgc_parser.init(&oPort);
    
    //centerGimbal();
}

void gim_control::angleControl(vector<int> pitchYawAngles)
{
    cout << "Sending cmd 1" << endl;
    cmd_control.data[PITCH].angle = 30;
    cmd_control.data[YAW].angle = 30;
    SBGC_cmd_control_ext_send(cmd_control, oSbgc_parser);

    //test moving gimbal
    /*
    SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(15);
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(15);
    SBGC_cmd_control_send(c, oSbgc_parser);
    cout << "sent command 2" << endl;
    */
}

void gim_control::centerGimbal()
{
    cmd_control.data[PITCH].angle = 30;
    cmd_control.data[YAW].angle = 30;
    SBGC_cmd_control_ext_send(cmd_control, oSbgc_parser);

    /*
    //move gimbal to initial position
    SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.speedROLL = c.speedPITCH = c.speedYAW = 30 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
    cout << "sent command center" << endl;
    usleep(500000);
    */
}
