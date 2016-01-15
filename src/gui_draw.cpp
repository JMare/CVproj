// Written by James Mare
// OpenCV program with goal of following a laser pointer
// gui_draw.cpp - function definitions for drawing gui
// includes processing loops for adjusting thresholding

//----PROJECT HEADERS-----------
#include "gui_draw.h"
#include "cvproj.h"
#include "im_proc.h"

//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>
#include <string>

//----NAMESPACES----------------
using namespace std;
using namespace cv;

//-------PUBLIC FUNCTIONS-------------------
gui_draw::gui_draw()
{//constructor - not used

}

void gui_draw::draw_interface()
{
    if(TRACKBAR_ENABLE){
        create_trackbars();
    }
    
    im_proc imFrame (camID); //instantiate object

    //this is the main proccessing loop for adjusting the image thresholding
    //in real time and viewing the results
    while(1){
        Mat frame;
        frame = imFrame.process_frame();

        display_image("Thresholded Frame", frame); 
        display_image("Main Feed", mainfeed);
        
        int key;

        key = cvWaitKey(10);     //Capture Keyboard stroke
        if (char(key) == 27){
            break;      //If you hit ESC key loop will break.
        }
    }
}


//-------PRIVATE FUNCTRIONS----------------
void gui_draw::create_trackbars()
{
    const string windowTrackbars = "Trackbars";
    namedWindow(windowTrackbars, 1);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	sprintf( TrackbarName, "Dilate Pixels", DILATE_PIX);
	sprintf( TrackbarName, "Erode Pixels", ERODE_PIX);
	sprintf( TrackbarName, "Dilate Iterations", DILATE_ITERATIONS);
	sprintf( TrackbarName, "Erode Iterations", ERODE_ITERATIONS);

	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", windowTrackbars, &H_MIN, 256);
    createTrackbar( "H_MAX", windowTrackbars, &H_MAX, 256);
    createTrackbar( "S_MIN", windowTrackbars, &S_MIN, 256);
    createTrackbar( "S_MAX", windowTrackbars, &S_MAX, 256);
    createTrackbar( "V_MIN", windowTrackbars, &V_MIN, 256);
    createTrackbar( "V_MAX", windowTrackbars, &V_MAX, 256);
    createTrackbar( "Dilate Pixels", windowTrackbars, &DILATE_PIX, 10);
    createTrackbar( "Erode Pixels", windowTrackbars, &ERODE_PIX, 10);
    createTrackbar( "Dilate Iterations", windowTrackbars, &DILATE_ITERATIONS, 10);
    createTrackbar( "Erode Iterations", windowTrackbars, &ERODE_ITERATIONS, 10);

}

void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

