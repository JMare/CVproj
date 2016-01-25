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

    im_proc imFrame; //instantiate object

    try{
        imFrame.init_feed(camID); //try to open webcam/video
    } catch (int x){ //check if feed actually opened
        switch(x){
            case 1:
                cout << "Frame source could not be opened" << endl;
                break;
            case 6:
                cout << "output_cap could not be opened" << endl;
                break; 
        }
        cout << "ERROR CODE: " << x << endl;
        return;
    }

    Mat frame_overlay, frame_thresholded; //create Mats for storing images to display
    
    if(TRACKBAR_ENABLE){
                create_trackbars(&imParams);
    }

    //this is the main proccessing loop for adjusting the image thresholding
    //in real time and viewing the results
    while(1){
        
        try{
            imFrame.process_frame();
        } catch(int x){
            cout << "Feed closed unexpectadly" << endl;
            cout << "ERROR CODE: " << x << endl;
            break;
        }

        frame_overlay = imFrame.get_frame_overlay();
        
        if(TRACKBAR_ENABLE){
            frame_thresholded = imFrame.get_frame_thresholded(TRACKBAR_PARAMS);
            display_image("Thresholded Frame", frame_thresholded); 
        }
        

        display_image("Camera Feed with overlay", frame_overlay);
        
        int key;
        
        key = cvWaitKey(10);     //Capture Keyboard stroke
        if (char(key) == 27){
            if(TRACKBAR_ENABLE){
                        cout << "Printing imParams" << endl;
                        print_params(&imParams);
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

    sprintf( TrackbarName, "MIN"               , params->at(0));
    sprintf( TrackbarName, "MAX"               , params->at(1));
    sprintf( TrackbarName, "Erode Pixels"      , params->at(2));
    sprintf( TrackbarName, "Dilate Pixels"     , params->at(3));
    sprintf( TrackbarName, "Erode Iterations"  , params->at(4));
    sprintf( TrackbarName, "Dilate Iterations" , params->at(5));
    sprintf( TrackbarName, "MAX_NUM_OBJECTS"   , inspect_image_params.at(0));
    sprintf( TrackbarName, "MIN_OBJECT_AREA"   , inspect_image_params.at(1));
    sprintf( TrackbarName, "MAX_OBJECT_AREA"   , inspect_image_params.at(2));
    sprintf( TrackbarName, "CHECK_SQUARE_SIZE" , check_candidates_params.at(0));
    sprintf( TrackbarName, "H_MIN"             , check_candidates_params.at(1));
    sprintf( TrackbarName, "H_MAX"             , check_candidates_params.at(2));
    sprintf( TrackbarName, "S_MIN"             , check_candidates_params.at(3));
    sprintf( TrackbarName, "S_MAX"             , check_candidates_params.at(4));
    sprintf( TrackbarName, "MIN_GREEN_REQUIRED", check_candidates_params.at(5));

    //create trackbars and insert them into window
    //3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
    //the max value the trackbar can move (eg. H_HIGH), 
    //and the function that is called whenever the trackbar is moved(eg. on_trackbar)
    //                                  ---->    ---->     ---->      
    createTrackbar( "MIN"                , windowTrackbars, &params->at(0), 256);
    createTrackbar( "MAX"                , windowTrackbars, &params->at(1), 256);
    createTrackbar( "Erode Pixels"       , windowTrackbars, &params->at(2), 10);
    createTrackbar( "Dilate Pixels"      , windowTrackbars, &params->at(3), 10);
    createTrackbar( "Erode Iterations"   , windowTrackbars, &params->at(4), 10);
    createTrackbar( "Dilate Iterations"  , windowTrackbars, &params->at(5), 10);
    createTrackbar( "MAX_NUM_OBJECTS"    , windowTrackbars, &inspect_image_params.at(0), 100);
    createTrackbar( "MIN_OBJECT_AREA"    , windowTrackbars, &inspect_image_params.at(1), 400);
    createTrackbar( "MAX_OBJECT_AREA"    , windowTrackbars, &inspect_image_params.at(2), 1000);
    createTrackbar( "CHECK_SQUARE_SIZE"  , windowTrackbars, &check_candidates_params.at(0), 255);
    createTrackbar( "H_MIN"              , windowTrackbars, &check_candidates_params.at(1), 255);
    createTrackbar( "H_MAX"              , windowTrackbars, &check_candidates_params.at(2), 255);
    createTrackbar( "S_MIN"              , windowTrackbars, &check_candidates_params.at(3), 255);
    createTrackbar( "S_MAX"              , windowTrackbars, &check_candidates_params.at(4), 255);
    createTrackbar( "MIN_GREEN_REQUIRED" , windowTrackbars, &check_candidates_params.at(5), 255);

}
void gui_draw::print_params(vector<int> *params)
{
    if(TRACKBAR_ENABLE){ //Print the final values in case we want to save them
        cout << "Final values: " << endl;
        cout << "{";
        for(int i=0; i<params->size(); i++){
            cout << params->at(i);
            if(i<params->size() -1) cout << ",";
        }
        cout << "}" << endl;

        cout << "{";
        for(int i=0; i<inspect_image_params.size(); i++){
            cout << inspect_image_params.at(i);
            if(i<inspect_image_params.size() -1) cout << ",";
        }
        cout << "}" << endl;

        cout << "{";
        for(int i=0; i<check_candidates_params.size(); i++){
            cout << check_candidates_params.at(i);
            if(i<check_candidates_params.size() -1) cout << ",";
        }
        cout << "}" << endl;
    }
}

void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

