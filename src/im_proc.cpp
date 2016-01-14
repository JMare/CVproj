// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.cpp - defines class functions for loading
// and manipulating images, and object detection

//----PROJECT HEADERS-----------
#include "im_proc.h"
#include "cvproj.h"

//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

//----NAMESPACES----------------
using namespace std;
using namespace cv;


double posX  = -1;
double posY  = -1;

//-------PUBLIC FUNCTIONS-------------------
im_proc::im_proc(int ID)
{//constructor
    //initialise a webcam
    

    cap = VideoCapture(ID); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {
        cout << "webcam could not be opened" << endl;
    }

}

Mat im_proc::getrawframe()
{

    Mat frame;
    frame = loadframewebcam();
    
    if(! frame.data){
        cout << "Did not get frame" << endl;
        return -1;
    }

    return frame;

}

Mat im_proc::getprocessed_frame()
{
    Mat frame;
    mainfeed = loadframewebcam();
    
    Mat frame_filtered;

    frame_filtered = threshold_frame(mainfeed);
    Mat frame_morphed;

    frame_morphed = morph_frame(frame_filtered);
    
    trackObject(frame_morphed);
    if(STREAM_POSITION){
    cout << "PosX = " << posX << " PosY = " << posY << endl;
    } 

    if(posX >=0 && posY >=0){
    Mat frame_overlay;

    mainfeed = overlay_position(mainfeed);
    }
    return frame_morphed;

}

//-------PRIVATE FUNCTRIONS----------------
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

Mat im_proc::loadframewebcam()
{
    Mat frame;
    cap >> frame;

    if(! frame.data )                              // Check for invalid input
    {
        cout <<  "No frame from webcam" << endl ;
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
    //OpenCV will crash if it tries to erode or dilate with
    //a pixel size of zero, if statements stop this.
    if(ERODE_PIX != 0){
        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(ERODE_PIX,ERODE_PIX));

        erode(frame,frame,erodeElement);
        erode(frame,frame,erodeElement);
    }
    
    if(DILATE_PIX != 0){
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(DILATE_PIX,DILATE_PIX));
        dilate(frame,frame,dilateElement);
        dilate(frame,frame,dilateElement);
    }
    return frame;
}

void im_proc::trackObject(Mat frame)
{
    Moments oMoments = moments(frame);

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    //calculate the position of the  object
    posX = dM10 / dArea;
    posY = dM01 / dArea;        
}

Mat im_proc::overlay_position(Mat frame)
{
   Mat object_overlay;

    circle(frame,Point(posX,posY),20,Scalar(0,0,255),2);

    Mat frame_overlay = frame + object_overlay;

    return frame_overlay;
}

