//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    oSbgc_parser.init(&oPort);

    //are we connected??
    //this does not work currently 
    // bool connectFlag = checkConnection();
    // cout << connectFlag << endl;
        
    //move gimbal to initial position
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    //set speed
    c.speedROLL = c.speedPITCH = c.speedYAW = 70 * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);

}

void gim_control::followPosition(tuple<bool, double, double> Pos)
{
    
    const double MOVEMENT_INT = 100; //unit is ms 

    now = time(NULL);

    //below is a crude example to demonstrate laser control
    if(difftime(now,time_last_movement) * 1000 >  MOVEMENT_INT){  
        time_last_movement = time(NULL);
        if(get<0>(Pos))
        {
            if(get<1>(Pos) < (FRAME_COLS / 2) -100)
            {
                absoluteAngleControl({0,-10});
            } else if (get<1>(Pos) > (FRAME_COLS /2) +100) 
            {
                absoluteAngleControl({0,10});
            }
        }
    }
}

vector<int> gim_control::calcRelativePosition(tuple<bool, double, double> Pos)
{
    int x = get<1>(Pos);
    int y = get<2>(Pos);

    const int GAIN_X = 0.05;
    const int GAIN_Y = 0.05;

    //These calculate a score between -100 and 100 for x and y
    int xCorrected = ((x - (FRAME_COLS / 2)) / (FRAME_COLS / 2)) * 100;
    int yCorrected = ((y - (FRAME_ROWS / 2)) / (FRAME_ROWS / 2)) * 100;
   
    int xRelativeAngle = GAIN_X * xCorrected;
    int yRelativeAngle = GAIN_Y * yCorrected;

    return {yRelativeAngle, xRelativeAngle};
}

void gim_control::relateiveAngleControl(vector<int> pitchYawAngles)
{
   int xAngleCmd = pitchYawAngles.at(1) + xAngleHistory;
   int yAngleCmd = pitchYawAngles.at(0) + yAngleHistory; 

   absoluteAngleControl({yAngleCmd, xAngleCmd});

   xAngleHistory = xAngleCmd;
   yAngleHistory = yAngleCmd;
}

void gim_control::absoluteAngleControl(vector<int> pitchYawAngles)
{
    //takes in {pitch,yaw} 
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(0));
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(1));
    SBGC_cmd_control_send(c, oSbgc_parser);
}

void gim_control::centerGimbal()
{
    //return to initial position
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

