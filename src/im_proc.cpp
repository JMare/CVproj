#include "im_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

extern int H_MIN;
extern int H_MAX;
extern int S_MIN;
extern int S_MAX;
extern int V_MIN;
extern int V_MAX;


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

Mat im_proc::getthreshold_frame(const char* fname)
{
    Mat frame;
    frame = loadframefile(fname);
    
    if(! frame.data){
        cout << "Did not get frame" << endl;
        return -1;
    }

    Mat frame_filtered;

    frame_filtered = threshold_frame(frame);

    return frame_filtered;

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

Mat im_proc::threshold_frame(Mat frame)
{
    Mat frame_HSV;
    Mat frame_Threshold;

   
    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);


    inRange(frame_HSV,Scalar(H_MIN,S_MIN,V_MIN),
        Scalar(H_MAX,S_MAX,V_MAX),frame_Threshold);
    return frame_Threshold;
}

