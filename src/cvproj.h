// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.h - Header file defining global variables


#ifndef CVPROJ_H
#define CVPROJ_H

#include <string>
#include <vector>

extern std::vector<int> imParams1;
extern std::vector<int> imParams2;

extern int H_MIN;
extern int H_MAX;
extern int S_MIN;
extern int S_MAX;
extern int V_MIN;
extern int V_MAX;

extern int DILATE_PIX;
extern int ERODE_PIX;
extern int DILATE_ITERATIONS;
extern int ERODE_ITERATIONS;

extern int FRAME_SOURCE;
extern int TRACKBAR_PARAMS;
extern char* FILENAME;
extern int camID;
extern bool TRACKBAR_ENABLE;
extern bool STREAM_POSITION;

extern cv::Mat mainfeed;

#endif 
