//Written by James Mare
//Handles serial control of the alexmos gimbal

#include "gim_control.h"

using namespace std;

gim_control::gim_control()
{
    cout << "trying serial" << endl;
    
    serial_port oPort;

    oPort.writeByte(41);

    uint16_t bytesavail = oPort.getBytesAvailable();

    cout << bytesavail << " bytes available" << endl;

    uint8_t read = oPort.readByte();
    

    cout << read << endl;

}
