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

    SBGC_cmd_control_t c = { 0, 0, 0, 0, 0, 0, 0 };

    //move gimbal to initial position
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.speedROLL = c.speedPITCH = c.speedYAW = 30 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
    usleep(5000000);

    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(40);
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(30);

    SBGC_cmd_control_send(c, oSbgc_parser);
    
    cout << "sent command 1" << endl;

    usleep(5000000);

    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(-40);
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(-30);

    SBGC_cmd_control_send(c, oSbgc_parser);
    cout << "sent command 2" << endl;

    cout << "ending serial" << endl;
    c.anglePITCH = 0;
    c.angleYAW = 0;
    SBGC_cmd_control_send(c, oSbgc_parser);
    usleep(5000000);
}
