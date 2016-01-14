// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.cpp - highest level code


//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

//----PROJECT HEADERS-----------
#include "im_proc.h"
#include "gui_draw.h"
#include "cvproj.h"

//----NAMESPACES----------------
using namespace cv;
using namespace std;

//global variables for thresholding editable with sliders
int H_MIN = 41;
int H_MAX = 57;
int S_MIN = 62;
int S_MAX = 196;
int V_MIN = 107;
int V_MAX = 238;

int ERODE_PIX  = 2;
int DILATE_PIX = 10;

int ERODE_ITERATIONS  = 2;
int DILATE_ITERATIONS = 2;

//command line arguments
int FRAME_SOURCE = 0; //0: webcam 1: video 2: image
int camID = 0;
char* FILENAME;

bool TRACKBAR_ENABLE = false;
bool STREAM_POSITION = false;

//This is the only global mat
Mat mainfeed;

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
        else if(string(argv[i]) == "-t") {
           TRACKBAR_ENABLE = true; 
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
    
    if(TRACKBAR_ENABLE){ //Print the final values in case we want to save them
        cout << "Final values: " << endl;

        cout << "H_MIN: " << H_MIN << endl;
        cout << "H_MAX: " << H_MAX << endl;
        cout << "S_MIN: " << S_MIN << endl;
        cout << "S_MAX: " << S_MAX << endl;
        cout << "V_MIN: " << V_MIN << endl;
        cout << "V_MAX: " << V_MAX << endl;

        cout << "ERODE_PIX: " << ERODE_PIX << endl;
        cout << "DILATE_PIX: " << DILATE_PIX << endl;

        cout << "ERODE_ITERATIONS: " << ERODE_ITERATIONS << endl;
        cout << "DILATE_ITERATIONS: " << DILATE_ITERATIONS << endl;

    }
    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

