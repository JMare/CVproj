// Written by James Mare
// Goal is to manipulate the webcam stream

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "im_proc.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    Mat testmat;

    im_proc imFrame;
    testmat = imFrame.getrawframe("test.jpg");

    cout << testmat << endl;
    
    cout << "Main run to completion - Aborting" << endl;
    return 0;

}

