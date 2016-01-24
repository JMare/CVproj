// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.h - Header file defining global variables


#ifndef CVPROJ_H
#define CVPROJ_H

#include <string>
#include <vector>

//paramater arrays
extern std::vector<int> imParams;
extern std::vector<int> inspect_image_params;
extern std::vector<int> check_candidates_params;

//Command line variables
extern int FRAME_SOURCE;
extern int TRACKBAR_PARAMS;
extern char* FILENAME;
extern int camID;
extern bool TRACKBAR_ENABLE;
extern bool STREAM_POSITION;
extern std::tuple<bool, double, double> Posmaster;
#endif 
