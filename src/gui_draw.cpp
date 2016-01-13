#include "gui_draw.h"
#include "cvproj.h"
#include "im_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cv;

gui_draw::gui_draw()
{//constructor
}

void gui_draw::draw_interface()
{
    create_trackbars();
    while(1){
        Mat frame;
        im_proc imFrame;
        frame = imFrame.getthreshold_frame("test.jpg");

        display_image("Thresholded Frame", frame); 

        int key;

        key = cvWaitKey(10);     //Capture Keyboard stroke
        if (char(key) == 27){
            break;      //If you hit ESC key loop will break.
        }
    }
}

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
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", windowTrackbars, &H_MIN, H_MAX);
    createTrackbar( "H_MAX", windowTrackbars, &H_MAX, H_MAX);
    createTrackbar( "S_MIN", windowTrackbars, &S_MIN, S_MAX);
    createTrackbar( "S_MAX", windowTrackbars, &S_MAX, S_MAX);
    createTrackbar( "V_MIN", windowTrackbars, &V_MIN, V_MAX);
    createTrackbar( "V_MAX", windowTrackbars, &V_MAX, V_MAX);

}

void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

