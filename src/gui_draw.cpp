// Written by James Mare
// OpenCV program with goal of following a laser pointer
// gui_draw.cpp - function definitions for drawing gui
// includes processing loops for adjusting thresholding

//----PROJECT HEADERS-----------
#include "gui_draw.h"
#include "cvproj.h"
#include "im_proc.h"

//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>
#include <string>

//----NAMESPACES----------------
using namespace std;
using namespace cv;

//-------PUBLIC FUNCTIONS-------------------
gui_draw::gui_draw()
{//constructor - not used

}

void gui_draw::draw_interface()
{
    if(TRACKBAR_ENABLE){
        create_trackbars();

        display_image("Thresholded Frame", frame_thresholded); 
    }

    display_image("Camera Feed with overlay", frame_overlay);
}

//-------PRIVATE FUNCTRIONS----------------
void gui_draw::create_trackbars()
{
    const string windowTrackbars = "ThresholdParams";
    namedWindow(windowTrackbars, 1);
    //create memory to store trackbar name on window
    char TrackbarName[50];

    sprintf( TrackbarName, "MIN"               , gParams.greyThreshMin);
    sprintf( TrackbarName, "MAX"               , gParams.greyThreshMax);
    sprintf( TrackbarName, "Erode Pixels"      , gParams.greyErodePix);
    sprintf( TrackbarName, "Dilate Pixels"     , gParams.greyDilatePix);
    sprintf( TrackbarName, "Erode Iterations"  , gParams.greyErodeIterations);
    sprintf( TrackbarName, "Dilate Iterations" , gParams.greyDilateIterations);
    sprintf( TrackbarName, "MAX_NUM_OBJECTS"   , gParams.candMaxNumObjects);
    sprintf( TrackbarName, "MIN_OBJECT_AREA"   , gParams.candMinObjectArea);
    sprintf( TrackbarName, "MAX_OBJECT_AREA"   , gParams.candMaxObjectArea);
    sprintf( TrackbarName, "CHECK_SQUARE_SIZE" , gParams.checkSquareSize);
    sprintf( TrackbarName, "H_MIN"             , gParams.checkHMin);
    sprintf( TrackbarName, "H_MAX"             , gParams.checkHMax);
    sprintf( TrackbarName, "S_MIN"             , gParams.checkSMin);
    sprintf( TrackbarName, "S_MAX"             , gParams.checkSMax);
    sprintf( TrackbarName, "MIN_GREEN_REQUIRED", gParams.checkMinGreen);

    //create trackbars and insert them into window
    //3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
    //the max value the trackbar can move (eg. H_HIGH), 
    //and the function that is called whenever the trackbar is moved(eg. on_trackbar)
    //                                  ---->    ---->     ---->      
    createTrackbar( "MIN"                , windowTrackbars, &gParams.greyThreshMin        , 256);
    createTrackbar( "MAX"                , windowTrackbars, &gParams.greyThreshMax        , 256);
    createTrackbar( "Erode Pixels"       , windowTrackbars, &gParams.greyErodePix         , 10);
    createTrackbar( "Dilate Pixels"      , windowTrackbars, &gParams.greyDilatePix        , 10);
    createTrackbar( "Erode Iterations"   , windowTrackbars, &gParams.greyErodeIterations  , 10);
    createTrackbar( "Dilate Iterations"  , windowTrackbars, &gParams.greyDilateIterations , 10);
    createTrackbar( "MAX_NUM_OBJECTS"    , windowTrackbars, &gParams.candMaxNumObjects, 100);
    createTrackbar( "MIN_OBJECT_AREA"    , windowTrackbars, &gParams.candMinObjectArea, 400);
    createTrackbar( "MAX_OBJECT_AREA"    , windowTrackbars, &gParams.candMaxObjectArea, 1000);
    createTrackbar( "CHECK_SQUARE_SIZE"  , windowTrackbars, &gParams.checkSquareSize, 255);
    createTrackbar( "H_MIN"              , windowTrackbars, &gParams.checkHMin    , 255);
    createTrackbar( "H_MAX"              , windowTrackbars, &gParams.checkHMax    , 255);
    createTrackbar( "S_MIN"              , windowTrackbars, &gParams.checkSMin    , 255);
    createTrackbar( "S_MAX"              , windowTrackbars, &gParams.checkSMax    , 255);
    createTrackbar( "MIN_GREEN_REQUIRED" , windowTrackbars, &gParams.checkMinGreen, 255);

    const string windowTrackbars2 = "Scoring Params";
    namedWindow(windowTrackbars2, 1);
    //create memory to store trackbar name on window
    char TrackbarName2[50];

    sprintf( TrackbarName, "calcScorePercentColor     ", gParams.calcScorePercentColor);
    sprintf( TrackbarName, "calcScorePercentColorExtra", gParams.calcScorePercentColorExtra);
    sprintf( TrackbarName, "calcScorePercentSize      ", gParams.calcScorePercentSize);
    sprintf( TrackbarName, "calcScorePercentCircle    ", gParams.calcScorePercentCircle);
    sprintf( TrackbarName, "calcAreaExpected          ", gParams.calcAreaExpected);
    sprintf( TrackbarName, "calcAreaMaxDiff           ", gParams.calcAreaMaxDiff);
    sprintf( TrackbarName, "calcMinPairDist           ", gParams.calcMinPairDist);
    sprintf( TrackbarName, "calcMaxPairDist           ", gParams.calcMaxPairDist);
    sprintf( TrackbarName, "masterMinScoreNonPair     ", gParams.masterMinScoreNonPair);
    sprintf( TrackbarName, "masterMinScorePair        ", gParams.masterMinScorePair);
    sprintf( TrackbarName, "masterPairScoreBoost      ", gParams.masterPairScoreBoost);

    createTrackbar( "calcScorePercentColor     ",windowTrackbars2, &gParams.calcScorePercentColor, 100);
    createTrackbar( "calcScorePercentColorExtra",windowTrackbars2, &gParams.calcScorePercentColorExtra, 100);
    createTrackbar( "calcScorePercentSize      ",windowTrackbars2, &gParams.calcScorePercentSize, 100);
    createTrackbar( "calcScorePercentCircle    ",windowTrackbars2, &gParams.calcScorePercentCircle, 100);
    createTrackbar( "calcAreaExpected          ",windowTrackbars2, &gParams.calcAreaExpected, 1000);
    createTrackbar( "calcAreaMaxDiff           ",windowTrackbars2, &gParams.calcAreaMaxDiff, 500);
    createTrackbar( "calcMinPairDist           ",windowTrackbars2, &gParams.calcMinPairDist, 100);
    createTrackbar( "calcMaxPairDist           ",windowTrackbars2, &gParams.calcMaxPairDist, 200);
    createTrackbar( "masterMinScoreNonPair     ",windowTrackbars2, &gParams.masterMinScoreNonPair, 100);
    createTrackbar( "masterMinScorePair        ",windowTrackbars2, &gParams.masterMinScorePair, 100);
    createTrackbar( "masterPairScoreBoost      ",windowTrackbars2, &gParams.masterPairScoreBoost, 100);

}

void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

