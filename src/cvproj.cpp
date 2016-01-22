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
//{0    , 1    , 2    , 3    , 4    , 5} 
//{Threshmin, thresmax, erodesize , dilatesize, erodeiter,dilateiter}
vector<int> imParams = {0, 256, 2, 4, 1, 2}; 


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

    gui_draw gui_obj; //create object for gui drawing

    gui_obj.draw_interface(); //this opens the main proccessing loop
    
    cout << "Main run to completion - Aborting" << endl;
    return 0;
}


