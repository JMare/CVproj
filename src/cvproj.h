// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.h - Header file defining global variables


#ifndef CVPROJ_H
#define CVPROJ_H

#include <string>
#include <vector>

//paramater arrays
extern std::vector<int> imParams1;
extern std::vector<int> imParams2;

//Command line variables
extern int FRAME_SOURCE;
extern int TRACKBAR_PARAMS;
extern char* FILENAME;
extern int camID;
extern bool TRACKBAR_ENABLE;
extern bool STREAM_POSITION;

#endif 
