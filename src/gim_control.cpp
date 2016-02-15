//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    softPwmCreate(23,50,100);
    usleep(10000000);
    softPwmWrite(23, 100);

    oSbgc_parser.init(&oPort);

    cout << "Gimbal Initialised" << endl;
    //are we connected??
    //this does not work currently 
    // bool connectFlag = checkConnection();
    // cout << connectFlag << endl;
        
    //move gimbal to initial position
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    //set speed
    c.speedROLL = c.speedPITCH = c.speedYAW = SPEED_SCALE_FACTOR * SBGC_SPEED_SCALE;
    SBGC_cmd_control_send(c, oSbgc_parser);
    usleep(1000000);

    
    for(uint8_t i=0; i<SBGC_API_VIRT_NUM_CHANNELS; i++) {
                    v.data[i] = SBGC_RC_UNDEF;
            }
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
    const double GAIN_X = 0.06;
    const double GAIN_Y = 0.03;

    //These calculate a score between -100 and 100 for x and y
    double xCorrected = ((x - (FRAME_COLS / 2)) / (FRAME_COLS / 2)) * 100;
    double yCorrected = ((y - (FRAME_ROWS / 2)) / (FRAME_ROWS / 2)) * 100;
   
    double xRelativeAngle, yRelativeAngle;

    if(xCorrected > 20 || xCorrected < -20){ 
        xRelativeAngle = GAIN_X * xCorrected;
    } else xRelativeAngle = 0;

    if(yCorrected > 20 || xCorrected < -20){ 
        yRelativeAngle = GAIN_Y * yCorrected;
    } else  yRelativeAngle = 0;

    return {yRelativeAngle, xRelativeAngle};
}

void gim_control::relateiveAngleControl(vector<double> pitchYawAngles)
{
   double xAngleCmd = pitchYawAngles.at(1) + xAngleHistory;
   double yAngleCmd = pitchYawAngles.at(0) + yAngleHistory;

   if(xAngleCmd < -YAW_LOWER_LIMIT) xAngleCmd = -YAW_LOWER_LIMIT;
   if(xAngleCmd > YAW_UPPER_LIMIT) xAngleCmd = YAW_UPPER_LIMIT;
   if(yAngleCmd < -PITCH_LOWER_LIMIT) yAngleCmd = -PITCH_LOWER_LIMIT;
   if(yAngleCmd > PITCH_UPPER_LIMIT) yAngleCmd = PITCH_UPPER_LIMIT;

   absoluteAngleControl({yAngleCmd, xAngleCmd});

   xAngleHistory = xAngleCmd;
   yAngleHistory = yAngleCmd;

}

void gim_control::absoluteAngleControl(vector<double> pitchYawAngles)
{
    //takes in {pitch,yaw}
    c.mode = SBGC_CONTROL_MODE_ANGLE;
    c.anglePITCH = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(0));
    c.angleYAW = SBGC_DEGREE_TO_ANGLE(pitchYawAngles.at(1));
    SBGC_cmd_control_send(c, oSbgc_parser);
}

void gim_control::pwmAngleControl(vector<double> pitchYawAngles)
{
    
}

void gim_control::centerGimbal()
{
    //return to initial position
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

long gim_control::myclock()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
}
