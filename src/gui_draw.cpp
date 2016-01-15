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
        create_trackbars(&imParams1);
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
            if(TRACKBAR_ENABLE){
                switch(TRACKBAR_PARAMS){
                    case 1:
                        cout << "Printing imParams1" << endl;
                        print_params(&imParams1);
                        break;
                    case 2:
                        cout << "Printing imParams2" << endl;
                        print_params(&imParams2);
                        break;
                } 
            }
            break;      //If you hit ESC key loop will break.
        }
    }
}


//-------PRIVATE FUNCTRIONS----------------
void gui_draw::create_trackbars(vector<int> *params)
{
    const string windowTrackbars = "Trackbars";
    namedWindow(windowTrackbars, 1);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", params->at(0));
	sprintf( TrackbarName, "H_MAX", params->at(1));
	sprintf( TrackbarName, "S_MIN", params->at(2));
	sprintf( TrackbarName, "S_MAX", params->at(3));
	sprintf( TrackbarName, "V_MIN", params->at(4));
	sprintf( TrackbarName, "V_MAX", params->at(5));
	sprintf( TrackbarName, "Dilate Pixels",     params->at(6));
	sprintf( TrackbarName, "Erode Pixels",      params->at(7));
	sprintf( TrackbarName, "Dilate Iterations", params->at(8));
	sprintf( TrackbarName, "Erode Iterations",  params->at(9));

	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", windowTrackbars, &params->at(0), 256);
    createTrackbar( "H_MAX", windowTrackbars, &params->at(1), 256);
    createTrackbar( "S_MIN", windowTrackbars, &params->at(2), 256);
    createTrackbar( "S_MAX", windowTrackbars, &params->at(3), 256);
    createTrackbar( "V_MIN", windowTrackbars, &params->at(4), 256);
    createTrackbar( "V_MAX", windowTrackbars, &params->at(5), 256);
    createTrackbar( "Dilate Pixels", windowTrackbars,     &params->at(7)     , 10);
    createTrackbar( "Erode Pixels", windowTrackbars,      &params->at(6)     , 10);
    createTrackbar( "Dilate Iterations", windowTrackbars, &params->at(6)     , 10);
    createTrackbar( "Erode Iterations", windowTrackbars,  &params->at(6)     , 10);

}
void gui_draw::print_params(vector<int> *params)
{
    if(TRACKBAR_ENABLE){ //Print the final values in case we want to save them
        cout << "Final values: " << endl;

        cout << "{"  << params->at(0);
        for(int i=1; i<10; i++){
            cout << ", " << params->at(i);
        }
        cout << "}" << endl;
    }
}
void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

