// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.h - Header file defining global variables


#ifndef CVPROJ_H
#define CVPROJ_H

#include <string>
#include <vector>
#include "params.h"
#include "im_proc.h"
extern params gParams;

extern bool isTracking;

//Mats for displaying
extern cv::Mat frame_overlay, frame_thresholded;

//Command line variables
extern int FRAME_SOURCE;
extern int TRACKBAR_PARAMS;
extern char* FILENAME;
extern int camID;
extern bool TRACKBAR_ENABLE;
extern bool STREAM_POSITION;
extern bool GIMBAL_ENABLE;

//Misc Functions
void print_debug(laserInfo* laser);
static long myclock();

#endif
