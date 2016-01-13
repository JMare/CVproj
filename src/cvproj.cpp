// Written by James Mare
// OpenCV program with goal of following a laser pointer
// cvproj.cpp - highest level code


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "im_proc.h"

#include "gui_draw.h"

#include "cvproj.h"

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

Mat mainfeed;

int main(int argc, char* argv[])
{

    gui_draw gui_obj;

    gui_obj.draw_interface(); //this opens the main proccessing loop

    cout << "Main run to completion - Aborting" << endl;
    return 0;
}

