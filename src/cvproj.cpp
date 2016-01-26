// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.cpp - highest level code


//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>

//----PROJECT HEADERS-----------
#include "im_proc.h"
#include "gui_draw.h"
#include "cvproj.h"

//----NAMESPACES----------------
using namespace cv;
using namespace std;


Mat frame_overlay, frame_thresholded;

//global variables for thresholding editable with sliders
//{0    , 1    , 2    , 3    , 4    , 5} 
//{Threshmin, thresmax, erodesize , dilatesize, erodeiter,dilateiter}
vector<int> imParams = {0, 256, 2, 4, 1, 2}; 
//global variables for inspec_image
//{MAX_NUM_OBJECTS, MIN_OBJECT_AREA, MAX_OBJECT_AREA}
vector<int> inspect_image_params = {20, 50, 700};
//global variables for check_candiates
//{CHECK_SQUARE_SIZE, H_MIN, H_MAX, S_MIN, S_MAX, MIN_GREEN_REQUIRED}
vector<int> check_candidates_params = {10,40,75,20,255,30};


//command line arguments
int FRAME_SOURCE = 0; //0: webcam 1: video 2: image
int TRACKBAR_PARAMS; //sets which param vector will be changed by trackbars
int camID = 0; 
char* FILENAME; //image or video file 
bool TRACKBAR_ENABLE = false;
bool STREAM_POSITION = false;


std::tuple<bool, double, double> Posmaster = make_tuple(true, 0, 0); 

int main(int argc, char* argv[])
{
    //dont let program start without arguments
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " --webcam ID" << endl;
        return 1;
    }

    //-------------------------GET COMMAND LINE ARGUMENTS-----------------
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--webcam") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                FRAME_SOURCE = 0;
                i++; //increment so we dont get the number next round
                camID = atoi(argv[i]); 
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--webcam option requires one argument." << endl;
                return 1;
            }  
        }
        else if (string(argv[i]) == "--vfile") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                FRAME_SOURCE = 1;
                i++; //increment so we dont get the number next round
                FILENAME = argv[i];
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--vfile option requires one argument." << endl;
                return 1;
            }  
        }
        else if (string(argv[i]) == "--ifile") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                FRAME_SOURCE = 2;
                i++; //increment so we dont get the number next round
                FILENAME = argv[i]; 
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--ifile option requires one argument." << endl;
                return 1;
            }  
        }
        if (string(argv[i]) == "--trackbar") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                TRACKBAR_ENABLE = true;
                i++; //increment so we dont get the number next round
                TRACKBAR_PARAMS = atoi(argv[i]); 
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--trackbar option requires one argument." << endl;
                return 1;
            }  
        }
        else if(string(argv[i]) == "--stream-position") {
           STREAM_POSITION = true; 
            }
    } 
    
    switch(FRAME_SOURCE){
        case 0:
            cout << "Cvproj started using wecam ID: " << camID << endl;
            break;
        case 1:
            cout << "Cvproj started using video file: " << FILENAME << endl;
            break;
        case 2:
            cout << "Cvproj started using image file: " << FILENAME << endl;
            break;
    }

    if(TRACKBAR_ENABLE) {
        cout << "Trackbars Enabled" << endl;
        cout << "Processing thread " << TRACKBAR_PARAMS << endl;
    }

    im_proc imFrame;

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
        return 0;
    }

    
    gui_draw gui_obj; //create object for gui drawing

    
    while(1){
        
        try{
            imFrame.process_frame();
        } catch(int x){
            cout << "Feed closed unexpectadly" << endl;
            cout << "ERROR CODE: " << x << endl;
            break;
        }
        
        frame_overlay = imFrame.get_frame_overlay();

        frame_thresholded = imFrame.get_frame_thresholded(TRACKBAR_PARAMS);

        gui_obj.draw_interface();

        int key;
        key = cvWaitKey(10);
        if (char(key) == 27){
            if(TRACKBAR_ENABLE){
                        cout << "Printing imParams" << endl;
                        gui_obj.print_params(&imParams);
            }
            break;      //If you hit ESC key loop will break.
        }

    }
    cout << "Main run to completion - Aborting" << endl;
    return 0;
}


