//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control_mc.h"

using namespace std;
using namespace boost;

string port = "/dev/ttyUSB0";
BufferedAsyncSerial serial(port,57600);

gim_control_mc::gim_control_mc()
{
}

void gim_control_mc::followPosition(laserInfo Pos)
{
now_ms = myclock();

        readRCSignals();
        readRCSignals();
        readRCSignals();

    if(now_ms - last_mov_ms >  gParams.gimMovementInt){  
        last_mov_ms = myclock();

        if(Pos.isMatch)
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

vector<double> gim_control_mc::calcRelativePosition(laserInfo Pos)
{
    double x = Pos.x;
    double y = Pos.y;

    //These scale the angle change linearly.
    double GAIN_X = float(gParams.gimGainX) / 1000;
    double GAIN_Y = float(gParams.gimGainY) /1000; 

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

void gim_control_mc::relateiveAngleControl(vector<double> pitchYawAngles)
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

void gim_control_mc::absoluteAngleControl(vector<double> pitchYawAngles)
{
    //turn angle into pwm % given known limits
    int pitchPwm = ((pitchYawAngles.at(0) - PITCH_LOWER_LIMIT) / (PITCH_UPPER_LIMIT - PITCH_LOWER_LIMIT)) * 100; 
    pitchPwm = 1000 + pitchPwm*10;

    int yawPwm = ((pitchYawAngles.at(1) - YAW_LOWER_LIMIT) / (YAW_UPPER_LIMIT - YAW_LOWER_LIMIT)) * 100; 
    yawPwm = 1000 + yawPwm*10;


    writeRCSignals(pitchPwm, yawPwm);


}

void gim_control_mc::writeRCSignals(int pitchPwm, int yawPwm)
{
    string pitchCmd;
    pitchCmd += "<1:";
    pitchCmd += to_string(pitchPwm);
    pitchCmd += ">";

    string yawCmd;
    yawCmd += "<2:";
    yawCmd += to_string(yawPwm);
    yawCmd += ">";

    cout << pitchCmd << yawCmd;
    try {
        serial.writeString(pitchCmd);
        serial.writeString("\n\r");
        serial.writeString(yawCmd);
        serial.writeString("\n\r");
    } catch(boost::system::system_error& e)
    {
        cout<<"Error: "<<e.what()<<endl;
    }
}


void gim_control_mc::readRCSignals(){
    string buffer;
    char c;
    bool endfound = false;

    buffer = serial.readStringUntil("\r\n");


    if(buffer.size() == 8)
    {
        if(buffer.at(0) == '<' && buffer.at(7) == '>'){ //then we got a complete command
            if(buffer.at(1) == '1'){
                char numberext[5] = {buffer.at(3), buffer.at(4), buffer.at(5), buffer.at(6), '\0'};
                PitchPwm = atoi(numberext);
            }
            else if(buffer.at(1) == '2'){
                char numberext[5] = {buffer.at(3), buffer.at(4), buffer.at(5), buffer.at(6), '\0'};
                YawPwm = atoi(numberext);
            }
            else if(buffer.at(1) == '3'){
                char numberext[5] = {buffer.at(3), buffer.at(4), buffer.at(5), buffer.at(6), '\0'};
                EnPwm = atoi(numberext);
                cout << EnPwm << endl;
            }
        }
    }
}


void gim_control_mc::centerGimbal()
{
    absoluteAngleControl({0,0});
}

long gim_control_mc::myclock()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
}
