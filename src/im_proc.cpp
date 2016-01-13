// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.cpp - defines class functions for loading
// and manipulating images, and object detection

#include "im_proc.h"
#include "cvproj.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;



im_proc::im_proc()
{//constructor
    //in future this will initialise a webcam    
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

Mat im_proc::getprocessed_frame(const char* fname)
{
    Mat frame;
    frame = loadframefile(fname);
    
    if(! frame.data){
        cout << "Did not get frame" << endl;
        return -1;
    }

    Mat frame_filtered;

    frame_filtered = threshold_frame(frame);
    Mat frame_morphed;

    frame_morphed = morph_frame(frame_filtered);

    return frame_morphed;

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

Mat im_proc::morph_frame(Mat frame)
{

    Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

    erode(frame,frame,erodeElement);
    erode(frame,frame,erodeElement);

    dilate(frame,frame,dilateElement);
    dilate(frame,frame,dilateElement);

    return frame;
}
