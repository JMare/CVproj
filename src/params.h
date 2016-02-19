#ifndef PARAMS_H
#define PARAMS_H

#include <string>
#include <map>
class params{
    public:
        params();

        void readParamsFile(std::string paramFileName);
        void writeParamsFile(std::string paramFileName);

        int greyThreshMin;
        int greyThreshMax;
        int greyErodePix;
        int greyDilatePix;
        int greyErodeIterations;
        int greyDilateIterations;
        
        int candMaxNumObjects;
        int candMinObjectArea;
        int candMaxObjectArea;

        int checkSquareSize;
        int checkHMin;
        int checkHMax;
        int checkSMin;
        int checkSMax;
        int checkMinGreen;

    private:
        std::map<std::string,int> paramMap;

};

#endif
