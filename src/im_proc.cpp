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
#include <string>
#include <vector>

//----NAMESPACES----------------
using namespace std;
using namespace cv;


//-------PUBLIC FUNCTIONS-------------------
im_proc::im_proc(int ID)
{//constructor
    //initialise a webcam
    
    if(FRAME_SOURCE == 0){
        cap = VideoCapture(ID); // open the default camera
        if(!cap.isOpened())  // check if we succeeded
        {
            cout << "webcam could not be opened" << endl;
        }
    }
    else if(FRAME_SOURCE == 1){
        cap = VideoCapture(FILENAME); // Open a video file
        if(!cap.isOpened())  // check if we succeeded
        {
            cout << "error when opening " << FILENAME << endl;
        }
    }

}

Mat im_proc::process_frame()
{
    loadframe(&mainfeed);
    
    Mat frame_proc1 = mainfeed.clone();
    Mat frame_proc2 = mainfeed.clone();
    
    threshold_frame(&frame_proc1, &imParams1);
    morph_frame(&frame_proc1, &imParams1);
    vector<double> Pos1 = trackObject(&frame_proc1);

    threshold_frame(&frame_proc2, &imParams2);
    morph_frame(&frame_proc2, &imParams2);
    vector<double> Pos2 = trackObject(&frame_proc2);
    
    return frame_proc1;
}

//-------PRIVATE FUNCTRIONS----------------
void im_proc::loadframe(Mat *frame)
{
    switch(FRAME_SOURCE){
        case 0:
            cap >> *frame;
            break;
        case 1:
            cap >> *frame;
            break;
        case 2:
            *frame = imread(FILENAME, CV_LOAD_IMAGE_COLOR);
    }

    if(! frame->data )                              // Check for invalid input
    {
        cout <<  "No frame returned" << endl ;
    }
}

void im_proc::threshold_frame(Mat *frame, vector<int> *params)
{
    //convert color to hsv
    cvtColor(*frame, *frame, COLOR_BGR2HSV);

    //apply thresholding
    inRange(*frame,Scalar(
                params->at(0),   //H_MIN
                params->at(2),   //S_MIN
                params->at(4)),  //V_MIN

        Scalar(
                params->at(1),  //H_MAX
                params->at(3),  //S_MAX
                params->at(5)), //V_MAX
        *frame);
}

void im_proc::morph_frame(Mat *frame, vector<int> *params)
{
    //OpenCV will crash if it tries to erode or dilate with
    //a pixel size of zero, if statements stop this.
    
    if(params->at(6) != 0){
        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(params->at(6),params->at(6)));
        
        for(int i=params->at(9); i>0; i--){
            erode(*frame,*frame,erodeElement);
        }
    }
    
    if(params->at(7) != 0){
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(params->at(7),params->at(7)));

        for(int i=params->at(8); i>0; i--){
            dilate(*frame,*frame,dilateElement);
        }
    }
}

vector<double> im_proc::trackObject(Mat *frame)
{
    Moments oMoments = moments(*frame);

    double posX;
    double posY;

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    //calculate the position of the  object
    posX = dM10 / dArea;
    posY = dM01 / dArea;        
    
    vector<double> position = {posX, posY};
    return position;
}

void im_proc::overlay_position(Mat *frame)
{
    //circle(*frame,Point(posX,posY),20,Scalar(0,0,255),2);
}

