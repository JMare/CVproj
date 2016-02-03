//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    oSbgc_parser.init(&oPort);

    //are we connected??
    
    bool connectFlag = checkConnection();
    cout << connectFlag << endl;
    
    //move gimbal to initial position
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    //set speed
    c.speedROLL = c.speedPITCH = c.speedYAW = 50 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
}

void gim_control::followPosition(tuple<bool, double, double> Pos)
{
    const int FRAME_ROWS = 480;
    const int FRAME_COLS = 680;
    
    if(get<0>(Pos))
    {
        if(get<1>(Pos) < (FRAME_COLS / 2) -100)
        {
            angleControl({0,-30});
        } else if (get<1>(Pos) > (FRAME_COLS /2) +100) 
        {
            angleControl({0,30});
        }
    }
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
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = 0;
    c.angleYAW = 0;
    SBGC_cmd_control_send(c, oSbgc_parser);
}

bool gim_control::checkConnection()
{
    SerialCommand cmd;
    cmd.init(SBGC_CMD_GET_ADJ_VARS_VAL);
    oSbgc_parser.send_cmd(cmd);

    usleep(10000);

    try{
        processIncomingMessages();
    }
    catch(int x){
        if(x == 51){
            return false;
        }
    } 
    return true;
}

void gim_control::processIncomingMessages()
{
    while(oSbgc_parser.read_cmd()) {
        SerialCommand &cmd = oSbgc_parser.in_cmd;
        
        uint8_t error = 0;
        int a = cmd.id;
        cout << "command id is: " << cmd.id << endl;

        switch(cmd.id) {
            case SBGC_CMD_REALTIME_DATA_4:
                error = SBGC_cmd_realtime_data_unpack(rt_data, cmd);
                if(error){
                     throw 51; 
                }
                break;
        }
    }
}

