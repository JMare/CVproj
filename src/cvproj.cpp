// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.cpp - highest level code


//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

//----PROJECT HEADERS-----------
#include "im_proc.h"
#include "gui_draw.h"
#include "gim_control.h"
#include "serial_port.h"
#include "cvproj.h"

//----NAMESPACES----------------
using namespace cv;
using namespace std;


Mat frame_overlay, frame_thresholded;

//global variables for thresholding editable with sliders
//{0    , 1    , 2    , 3    , 4    , 5} 
//{Threshmin, thresmax, erodesize , dilatesize, erodeiter,dilateiter}
vector<int> imParams = {0, 256, 2, 4, 0, 2}; 

//global variables for inspec_image
//{MAX_NUM_OBJECTS, MIN_OBJECT_AREA, MAX_OBJECT_AREA}
vector<int> inspect_image_params = {20, 50, 700};

//global variables for check_candiates
//{CHECK_SQUARE_SIZE, H_MIN, H_MAX, S_MIN, S_MAX, MIN_GREEN_REQUIRED}
vector<int> check_candidates_params = {10,40,75,20,255,30};


//command line arguments
int FRAME_SOURCE = 0; //0: webcam 1: video 2: image
int camID = 0; 
char* FILENAME; //image or video file 
bool TRACKBAR_ENABLE = false;
bool GUI_ENABLE = false;
bool DEBUG_FLAG = false;

//these probably shoulnt be doubles, it seems wasteful
double  now_ms = 0;
double  last_loop_ms = 0;
double  last_debug_ms = 0;
double  loop_begin_ms = 0;
const int LOOP_HISTORY_LENGTH = 20;
double loopTime;
const int DEBUG_INTERVAL = 2000;

vector<long int> loopTimeHistory;
tuple<bool, double, double> Pos;
std::tuple<bool, double, double> Posmaster = make_tuple(false, 0, 0); 

int numCandPass, areaOfLaser, greenOfLaser;

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
                TRACKBAR_ENABLE = true;
        }
        else if(string(argv[i]) == "--gui") {
            GUI_ENABLE  = true; 
            }
        else if(string(argv[i]) == "--debug") {
            DEBUG_FLAG = true; 
            }
    } 

    //Instantiate classes
    
    im_proc imFrame;

    gim_control oGim; 

    gui_draw gui_obj; //create object for gui drawing

    //Move the gimbal slightly and then back to center
    usleep(100000);
    oGim.absoluteAngleControl({5,5});
    usleep(100000);
    oGim.centerGimbal();
    usleep(100000);


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

    gui_obj.print_params(&imParams);

    cout << "Main processing loop started" << endl;

    while(1){
        //Calculate the loop time for the last loop
        last_loop_ms = loop_begin_ms;
        now_ms = myclock();
        loop_begin_ms = now_ms;
        loopTime = loop_begin_ms - last_loop_ms;
        
        //Add looptime to the looptimehistory vector
        loopTimeHistory.push_back(loopTime);
        if(loopTimeHistory.size() > LOOP_HISTORY_LENGTH)
        {
            //If full, push out the last entry
            loopTimeHistory.erase(loopTimeHistory.begin());
        }
        
        try{
            imFrame.process_frame();
        } catch(int x){
            cout << "Feed closed unexpectadly" << endl;
            cout << "ERROR CODE: " << x << endl;
            break;
        }
        
        frame_overlay = imFrame.get_frame_overlay();

        frame_thresholded = imFrame.get_frame_thresholded();
        
        Pos = imFrame.get_position();
        
        oGim.followPosition(Pos);
        
        if(GUI_ENABLE)
        {
            gui_obj.draw_interface();
        }

        now_ms = myclock();

        if(DEBUG_FLAG)
        {
            if(now_ms - last_debug_ms > DEBUG_INTERVAL)
            {
                print_debug();
                last_debug_ms = now_ms;
            }
        }

        int key;
        key = cvWaitKey(1);
       if (char(key) == 27){
            if(TRACKBAR_ENABLE){
                        cout << "Printing imParams" << endl;
                        gui_obj.print_params(&imParams);
            }
            break;      //If you hit ESC key loop will break.
        }

        imFrame.get_info( &numCandPass, &areaOfLaser, &greenOfLaser);

        now_ms = myclock();

    } //End main proccesing loop

    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

void print_debug()
{

cout << "------------------------" << endl;

//Calculate loop time
int loopTimePrint = (accumulate(loopTimeHistory.begin(),loopTimeHistory.end(),0)) / loopTimeHistory.size();
int loopTimeFPS = 1000 / loopTimePrint;

cout << "Loop time: " << loopTimePrint << " ms. FPS: " << loopTimeFPS <<  endl;
cout << "Laser candidates: " << numCandPass << endl;
cout << "Position found: " << get<0>(Pos) << endl;
cout << "Coordinates - x: " << get<1>(Pos) << " y: " << get<2>(Pos) << endl;


if(get<0>(Pos)){
    cout << "Laser Area: " << areaOfLaser << endl;
    cout << "Laser green count: " <<  greenOfLaser << endl;
}
    cout << "------------------------" << endl;
}


static long myclock()
{
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<float, std::milli> duration;

    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    return elapsed.count();
    //returns in ms
}
