#include "im_proc.h"
#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace std;


im_proc::im_proc()
{//constructor
    cv::VideoCapture capture;
    capture.open(0);
}



cv::Mat im_proc::getfeed()
{
    cv::Mat frame;
    capture.read(frame);
    return frame;

}
