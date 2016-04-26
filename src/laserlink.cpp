#include <iostream>
#include <string>
#include "laserlink.h"
#include "cvproj.h"

int parse_header(std::string inHeader, laserEnum* messageEnum)
{
    //parse the header string, check its length
    //return -1 if string invalid, otherwise return body length
    //return by refference the enum of what the message is
    
    if(inHeader.size() != 12) return -1;
    if(inHeader.at(0) != 'S' || 
            inHeader.at(1) != 'H' ||
            inHeader.at(2) != 'X' ||
            inHeader.at(9) != 'E' ||
            inHeader.at(10) != 'H' || 
            inHeader.at(11) != 'X')
    {
        return -1;
    }

    //get the body length
    std::string lengthString = inHeader.substr(3,3);
    int body_length = std::stoi(lengthString);

    std::string enumString = inHeader.substr(6,3);

    //Check that the string for message type matches,
    //then set the type enumeration by refference
    if(enumString == "PAR") *messageEnum = PAR;
    else if(enumString == "HBT") *messageEnum = HBT;
    else if(enumString == "DOC") *messageEnum = DOC;
    else if(enumString == "PRQ") *messageEnum = PRQ;
    else if(enumString == "PSE") *messageEnum = PSE;
    else if(enumString == "MOV") *messageEnum = MOV;
    else return -1;

    return body_length;
}

ll_do::ll_do(std::string inMessage, int bodylen)
{
    isValid = true;
    if(inMessage.size() != bodylen + 6) isValid = false;
    if(inMessage.at(0) != 'S' || 
            inMessage.at(1) != 'M' ||
            inMessage.at(2) != 'X' ||
            inMessage.at(9) != 'E' ||
            inMessage.at(10) != 'M' || 
            inMessage.at(11) != 'X')
    {
        isValid = false;
    }

    //get the parameter 
    std::string lengthString = inMessage.substr(6,3);
    param1 = std::stoi(lengthString);

    std::string enumString = inMessage.substr(3,3);

    if(enumString == "TOG") command = TOG;
    else if(enumString == "MOD") command = MOD;
    else isValid = false;
}

void ll_do::execute_command(void)
{
    if(command == TOG){
        if(param1 == 0){
            std::cout << "Turning the tracking off" << std::endl;
            isTracking = false;
        } else if(param1 == 1){
            std::cout << "Turning the tracking on" << std::endl;
            isTracking = true;
        }
    } else if(command == MOD){
        if(param1 == 0){
            //RC Control
        } else if(param1 == 1){
            //Touch Control
        } else if (param1 == 2){
            //Vision Control
        }
    }
}

ll_mov::ll_mov(std::string inMessage, int bodylen)
{
    isValid = true;
    if(inMessage.size() != bodylen + 6) isValid = false;
    if(inMessage.at(0) != 'S' || 
            inMessage.at(1) != 'M' ||
            inMessage.at(2) != 'X' ||
            inMessage.at(9) != 'E' ||
            inMessage.at(10) != 'M' || 
            inMessage.at(11) != 'X')
    {
        isValid = false;
    }

    unsigned int xSign, ySign;
    //get x
    std::string xString = inMessage.substr(4,2);
    xSign = std::stoi(inMessage.substr(3,1));
    if(xSign == 0){
        xMov = std::stoi(xString);
    } else {
        xMov = - std::stoi(xString);
    }
    //get y
    std::string yString = inMessage.substr(7,2);
    ySign = std::stoi(inMessage.substr(6,1));
    if(ySign == 0){
        yMov = std::stoi(yString);
    } else {
        yMov = - std::stoi(yString);
    }
}

void ll_mov::execute_command(void)
{
    //do stuff
    std::cout << "executing movment command: " << xMov << yMov << std:: endl;
}

