// Written by James Mare
// Goal is to manipulate the webcam stream

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "im_proc.h"

#include "gui_draw.h"

using namespace cv;
using namespace std;


int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

int main(int argc, char* argv[])
{
    Mat testmat;

    im_proc imFrame;
    testmat = imFrame.getthreshold_frame("test.jpg");

    gui_draw gui_obj;

    gui_obj.draw_interface(testmat);

    cout << "Main run to completion - Aborting" << endl;
    return 0;

}

