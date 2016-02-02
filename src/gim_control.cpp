//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    cout << "Starting serial" << endl;

    serial_port oPort;
    SBGC_Parser oSbgc_parser;
    SerialCommand b;

    oSbgc_parser.init(&oPort);

    SBGC_cmd_control_t c;

    /////////////////// Demo 1. PITCH and YAW gimbal by 40 and 30 degrees both sides and return back.
    // Actual speed depends on PID setting.
    // Whait 5 sec to finish
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(40);
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(30);

    SBGC_cmd_control_send(c, oSbgc_parser);

    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(-40);
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(-30);
    SBGC_cmd_control_send(c, oSbgc_parser);


    cout << "ending serial" << endl;

}
