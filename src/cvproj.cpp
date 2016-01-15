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

//global variables for thresholding editable with sliders

//{0    , 1    , 2    , 3    , 4    , 5    , 6        , 7         , 8                , 9}
//{H_MIN, H_MAX, S_MIN, S_MAX, V_MIN, V_MAX, ERODE_PIX, DILATE_PIX, DILATE_ITERATIONS, ERODE_ITERATIONS};
vector<int> imParams1 = {41, 57, 62, 196, 107, 238, 2, 10, 2, 2};
vector<int> imParams2 = {41, 57, 62, 196, 107, 238, 2, 10, 2, 2};

//command line arguments
int FRAME_SOURCE = 0; //0: webcam 1: video 2: image
int TRACKBAR_PARAMS;
int camID = 0;
char* FILENAME;

bool TRACKBAR_ENABLE = false;
bool STREAM_POSITION = false;


int main(int argc, char* argv[])
{
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " --webcam ID -t" << endl;
        return 1;
    }

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
    }

    gui_draw gui_obj;

    gui_obj.draw_interface(); //this opens the main proccessing loop
    
    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

