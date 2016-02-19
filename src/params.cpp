#include <iostream>
#include <fstream>
#include "params.h"
#include <string>
using namespace std;

params::params()
{
    greyThreshMin = 230;  
    greyThreshMax = 256; 
    greyErodePix = 2;  
    greyDilatePix = 4; 
    greyErodeIterations = 0;
    greyDilateIterations = 2; 

    candMaxNumObjects = 20; 
    candMinObjectArea = 50;
    candMaxObjectArea = 700; 

    checkSquareSize = 10; 
    checkHMin = 40; 
    checkHMax = 75; 
    checkSMin = 20;
    checkSMax = 255; 
    checkMinGreen = 30; 
}

void params::readParamsFile(string paramFileName)
{
    string line, paramref;
    ifstream myfile (paramFileName);
    int lineindex = 0;
    if (myfile.is_open())
    {
        while (getline(myfile,line))
        {
            if(lineindex%2 == 0){
                paramref = line;
            } else {
                paramMap[paramref] = stoi(line);
            }
            lineindex++;
        }
        myfile.close();
    }

    if(!(paramMap.find("greyThreshMin") == paramMap.end())){
        greyThreshMin = paramMap["greyThreshMin"];
    }
    if(!(paramMap.find("greyThreshMax") == paramMap.end())){
        greyThreshMax = paramMap["greyThreshMax"];
    }
    if(!(paramMap.find("greyErodePix") == paramMap.end())){
        greyErodePix = paramMap["greyErodePix"];
    }
    if(!(paramMap.find("greyDilatePix") == paramMap.end())){
        greyDilatePix = paramMap["greyDilatePix"];
    }
    if(!(paramMap.find("greyErodeIterations") == paramMap.end())){
        greyErodeIterations = paramMap["greyErodeIterations"];
    }
    if(!(paramMap.find("greyDilateIterations") == paramMap.end())){
        greyDilateIterations = paramMap["greyDilateIterations"];
    }
    if(!(paramMap.find("candMaxNumObjects") == paramMap.end())){
        candMaxNumObjects = paramMap["candMaxNumObjects"];
    }
    if(!(paramMap.find("candMinObjectArea") == paramMap.end())){
        candMinObjectArea = paramMap["candMinObjectArea"];
    }
    if(!(paramMap.find("candMaxObjectArea") == paramMap.end())){
        candMaxObjectArea = paramMap["candMaxObjectArea"];
    }
    if(!(paramMap.find("checkSquareSize") == paramMap.end())){
        checkSquareSize = paramMap["checkSquareSize"];
    }
    if(!(paramMap.find("checkHMin") == paramMap.end())){
        checkHMin = paramMap["checkHMin"];
    }
    if(!(paramMap.find("checkHMax") == paramMap.end())){
        checkHMax = paramMap["checkHMax"];
    }
    if(!(paramMap.find("checkSMin") == paramMap.end())){
        checkSMin = paramMap["checkSMin"];
    }
    if(!(paramMap.find("checkSMax") == paramMap.end())){
        checkSMax = paramMap["checkSMax"];
    }
    if(!(paramMap.find("checkMinGreen") == paramMap.end())){
        checkMinGreen = paramMap["checkMinGreen"];
    }
}

void params::writeParamsFile(string paramFileName)
{
    ofstream myfile;
    myfile.open (paramFileName);
    
    myfile << "greyThreshMin" << endl;
    myfile << greyThreshMin << endl;

    myfile << "greyThreshMax" << endl;
    myfile << greyThreshMax << endl;

    myfile << "greyErodePix" << endl;
    myfile << greyErodePix << endl;

    myfile << "greyDilatePix" << endl;
    myfile << greyDilatePix << endl;

    myfile << "greyErodeIterations" << endl;
    myfile << greyErodeIterations << endl;

    myfile << "greyDilateIterations" << endl;
    myfile << greyDilateIterations << endl;

    myfile << "candMaxNumObjects" << endl;
    myfile << candMaxNumObjects << endl;

    myfile << "candMinObjectArea" << endl;
    myfile << candMinObjectArea << endl;

    myfile << "candMaxObjectArea" << endl;
    myfile << candMaxObjectArea << endl;

    myfile << "checkSquareSize" << endl;
    myfile << checkSquareSize << endl;

    myfile << "checkHMin" << endl;
    myfile << checkHMin << endl;

    myfile << "checkHMax" << endl;
    myfile << checkHMax << endl;

    myfile << "checkSMin" << endl;
    myfile << checkSMin << endl;

    myfile << "checkSMax" << endl;
    myfile << checkSMax << endl;

    myfile << "checkMinGreen" << endl;
    myfile << checkMinGreen << endl;

    myfile.close();

}
