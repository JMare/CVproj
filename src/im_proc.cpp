#include "im_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;


im_proc::im_proc()
{//constructor
    
}



Mat im_proc::getrawframe(const char* fname)
{

    Mat frame;
    frame = loadframefile(fname);
    
    if(! frame.data){
        cout << "Did not get frame" << endl;
        return -1;
    }

    return frame;

}

Mat im_proc::loadframefile(const char* fname)
{
    Mat frame;
    frame = imread(fname, CV_LOAD_IMAGE_COLOR);

    if(! frame.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    return frame;
}
