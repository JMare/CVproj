// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.h - Header file defining global variables


#ifndef CVPROJ_H
#define CVPROJ_H

#include <string>
#include <vector>
#include <cstdlib>

//paramater arrays
extern std::vector<int> imParams;
extern std::vector<int> inspect_image_params;
extern std::vector<int> check_candidates_params;

extern cv::Mat frame_overlay, frame_thresholded;

//Command line variables
extern int FRAME_SOURCE;
extern int TRACKBAR_PARAMS;
extern char* FILENAME;
extern int camID;
extern bool TRACKBAR_ENABLE;
extern bool STREAM_POSITION;
extern std::tuple<bool, double, double> Posmaster;

im_proc imFrame;

gim_control oGim; 

gui_draw gui_obj; //create object for gui drawing

void print_debug();
static long myclock();
void onExit();

#endif 
