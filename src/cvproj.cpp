// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.cpp - highest level code


//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

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

int ERODE_PIX = 2;
int DILATE_PIX = 10;

int camID = 0;

bool TRACKBAR_ENABLE = false;
bool STREAM_POSITION = false;

Mat mainfeed;



int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " -v ID -t" << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-v") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                i++; //increment so we dont get the number next round
                camID = atoi(argv[i]); 
            } else { // Uh-oh, there was no argument to the destination option.
                  std::cerr << "-v option requires one argument." << std::endl;
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
    
    cout << "Program starting with camera ID " << camID << endl;

    if(TRACKBAR_ENABLE) {
        cout << "Trackbars Enabled" << endl;
    }

    gui_draw gui_obj;

    gui_obj.draw_interface(); //this opens the main proccessing loop

    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

