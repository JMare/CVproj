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
#include "gim_control_mc.h"
#include "params.h"
#include "cvproj.h"

//----NAMESPACES----------------
using namespace cv;
using namespace std;


params gParams;

Mat frame_overlay, frame_thresholded;

//command line arguments
int FRAME_SOURCE = 0; //0: webcam 1: video 2: image
int camID = 0; 
char* FILENAME; //image or video file 
bool TRACKBAR_ENABLE = false;
bool GUI_ENABLE = false;
bool DEBUG_FLAG = false; 
bool PARAMS_READ = false;
bool PARAMS_WRITE = false;
bool DO_CALIBRATION = false;
bool LOAD_CALIBRATION = false;
string PARAMS_IN_FILENAME;
string PARAMS_OUT_FILENAME;

//these probably shoulnt be doubles, it seems wasteful
double  now_ms = 0;
double  last_loop_ms = 0;
double  last_debug_ms = 0;
double  loop_begin_ms = 0;
double  last_write_ms = 0;
const int LOOP_HISTORY_LENGTH = 20;
double loopTime;
const int DEBUG_INTERVAL = 2000;
const int PARAM_WRITE_INTERVAL = 10000;

vector<long int> loopTimeHistory;

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
        if (string(argv[i]) == "--param-in") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                PARAMS_READ = true;
                i++; //increment so we dont get the number next round
                PARAMS_IN_FILENAME = argv[i]; 
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--param-in option requires one argument." << endl;
                return 1;
            }  
        }
        if (string(argv[i]) == "--param-out") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                PARAMS_WRITE = true;
                i++; //increment so we dont get the number next round
                PARAMS_OUT_FILENAME = argv[i]; 
            } else { // Uh-oh, there was no argument to the destination option.
                  cerr << "--param-out option requires one argument." << endl;
                return 1;
            }  
        }
        if (string(argv[i]) == "--trackbar") {
                TRACKBAR_ENABLE = true;
        }
        if (string(argv[i]) == "--readcal") {
                LOAD_CALIBRATION  = true;
        }
        if (string(argv[i]) == "--docal") {
                DO_CALIBRATION  = true;
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

    gui_draw gui_obj; //create object for gui drawing

    gim_control_mc oGim;

    if(PARAMS_READ){
        gParams.readParamsFile(PARAMS_IN_FILENAME);
    }

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

    if(DO_CALIBRATION){
        imFrame.calibrateImage();
        gParams.writeCalibFile("lastcalib.txt");
    } else if(LOAD_CALIBRATION){
        gParams.readParamsFile("lastcalib.txt");
    }

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
        
        laserInfo PosMain = imFrame.process_frame();
        
        frame_overlay = imFrame.get_frame_overlay();

        frame_thresholded = imFrame.get_frame_thresholded();
        
        oGim.followPosition(PosMain);
        
        if(GUI_ENABLE)
        {
            gui_obj.draw_interface();
        }

        now_ms = myclock();

        if(PARAMS_WRITE){
            if(now_ms - last_write_ms > PARAM_WRITE_INTERVAL){
                gParams.writeParamsFile(PARAMS_OUT_FILENAME);
                last_write_ms = now_ms;
            }
        }

        if(DEBUG_FLAG)
        {
            if(now_ms - last_debug_ms > DEBUG_INTERVAL)
            {
                print_debug(&PosMain);
                last_debug_ms = now_ms;
            }
        }

        int key;
        if(FRAME_SOURCE == 0){
            key = cvWaitKey(1);
        }
        else if(FRAME_SOURCE == 1){
            key = cvWaitKey(50);
        }
       if (char(key) == 27){
            break;      //If you hit ESC key loop will break.
        }

        now_ms = myclock();

    } //End main proccesing loop

    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

void print_debug(laserInfo* laser)
{

cout << "------------------------" << endl;

//Calculate loop time
int loopTimePrint = (accumulate(loopTimeHistory.begin(),loopTimeHistory.end(),0)) / loopTimeHistory.size();
int loopTimeFPS = 1000 / loopTimePrint;

cout << "Loop time: " << loopTimePrint << " ms. FPS: " << loopTimeFPS <<  endl;

cout << "Position found: " << laser->isMatch << endl;

if(laser->isMatch){
    cout << "Coordinates - x: " << laser->x << " y: " << laser->y << endl;
    cout << "Laser Area: " << laser->area << endl;
    cout << "Laser green count: " <<  laser->colorCount << endl;
    cout << "Laser total score: " << laser->matchScore << endl;
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

