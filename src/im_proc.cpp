// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.cpp - defines class functions for loading
// and manipulating images, and object detection

//----PROJECT HEADERS-----------
#include "im_proc.h"
#include "cvproj.h"

//----EXTERNAL HEADERS-----------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>

//----NAMESPACES----------------
using namespace std;
using namespace cv;

//-------PUBLIC FUNCTIONS-------------------
im_proc::im_proc(){
    //constructor
    //not used for now
}

void im_proc::init_feed(int ID)
{
    //first function to be run, this opens the capture object
    //only needed for video and webcam sources
    if(FRAME_SOURCE == 0){
        cap = VideoCapture(ID); // open the default camera
        if(!cap.isOpened())  // check if we succeeded
        {
            throw 1;
        }
    }
    else if(FRAME_SOURCE == 1){
        cap = VideoCapture(FILENAME); // Open a video file
        if(!cap.isOpened())  // check if we succeeded
        {
            throw 1;
        }
    }

    cout << "Welcome to CVproj. " << endl;
    cout << "Please ensure the laser is not in the frame." << endl;
    cout << "Press enter to continue." << endl;
    system("read");

    //discard first 60 frames to ensure webcam exposure has settled
    for( int i = 0; i <= 30; i++)
    {
        loadframe(&mainfeed);
    }

    bool emptyframe = false;

    while (!emptyframe)
    {
        loadframe(&mainfeed);
        Mat frame_proc = mainfeed.clone();

        threshold_frame(&frame_proc, &imParams);
        morph_frame(&frame_proc, &imParams);
        
        frame_info = inspect_frame(&frame_proc);

        double areafound = get<2>(frame_info);
        
        if(areafound == 0) emptyframe = true;

        imParams.at(0) = imParams.at(0) + 7;

    }

    cout << "Background calibration complete" << endl;
    cout << "Please turn on the laser and ensure it is in the frame" << endl;
    cout << "Press enter to continue" << endl;
    system("read");

    //now we need to check if we find the laser
    
    //discard 30 frames
    for( int i = 0; i <= 30; i++)
    {
        loadframe(&mainfeed);
    }
    
    bool laserfound = false;
    while(!laserfound)
    {
        loadframe(&mainfeed);
        Mat frame_proc = mainfeed.clone();
        threshold_frame(&frame_proc, &imParams);
        morph_frame(&frame_proc, &imParams);
        frame_info = inspect_frame(&frame_proc);
        
        int matchID = check_candidates(get<0>(frame_info));
        
        if(matchID >= 0) laserfound = true;
        else imParams.at(0) = imParams.at(0) - 2;

        cout << "lowered threshold" << endl;
    }

    //then after everything go down a bit more to make sure
    imParams.at(0) = imParams.at(0) - 5;
}

void im_proc::process_frame()
{
    loadframe(&mainfeed);

    //clone is nessasary, assignment does not copy
    frame_proc = mainfeed.clone();

    threshold_frame(&frame_proc, &imParams);
    morph_frame(&frame_proc, &imParams);

    frame_info = inspect_frame(&frame_proc);

    vector<vector<double>> candidatearray = get<0>(frame_info); 

    int matchID = check_candidates(candidatearray);
 
    if(matchID >= 0)
    {
        vector<double> matcharray = candidatearray.at(matchID);
        Pos = make_tuple(true, matcharray.at(1), matcharray.at(2));
    } else
    {
        Pos = make_tuple(false, -1, -1);
    }
}

Mat im_proc::get_frame_overlay()
{
    overlay_position(&mainfeed);
    return mainfeed;
}

Mat im_proc::get_frame_thresholded(int feedID)
{
            return frame_proc;
}
//-------PRIVATE FUNCTRIONS----------------


void im_proc::threshold_frame(Mat *frame, vector<int> *params)
{
    vector<int> paramslocal = *params;

    cvtColor(*frame, *frame, CV_RGB2GRAY);
    threshold(*frame, *frame, paramslocal.at(0), paramslocal.at(1), THRESH_BINARY);
}


void im_proc::morph_frame(Mat *frame, vector<int> *params)
{
    //OpenCV will crash if it tries to erode or dilate with
    //a pixel size of zero, if statements stop this.
    
    if(params->at(2) != 0){
        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(params->at(2),params->at(2)));
        
        //for iterations erode
        for(int i=params->at(4); i>0; i--){
            erode(*frame,*frame,erodeElement);
        }
    }
    
    if(params->at(3) != 0){
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(params->at(3),params->at(3)));
        
        //for iterations dilate
        for(int i=params->at(5); i>0; i--){
            dilate(*frame,*frame,dilateElement);
        }
    }
}


tuple< vector<vector<double>>, int, double> im_proc::inspect_frame(Mat *frame)
{

    Mat temp = frame->clone();

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
    const int MAX_NUM_OBJECTS = 20;
    const int MIN_OBJECT_AREA = 50;
    const int MAX_OBJECT_AREA = 500;

    int numObjects = hierarchy.size();

    double x, y;
    double totalArea = countNonZero(*frame);
    
    //a vector of vectors
    vector<vector<double>> candidates;

    if (numObjects > 0 && numObjects < MAX_NUM_OBJECTS)
    {
        for (int index = 0; index >= 0; index = hierarchy[index][0]) 
        {
            //for each object

            Moments moment = moments((cv::Mat)contours[index]);
            double area = moment.m00;
            
            if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA)
            {
                //if area matches, calculate position and
                //pack it into the vector of candidates
                x = moment.m10/area;
                y = moment.m01/area;
                vector<double> rowtoadd { area, x, y };
                candidates.push_back(rowtoadd);

            } 
        } 
    }
    
    return make_tuple(candidates, numObjects, totalArea);
}
            
int im_proc::check_candidates(vector<vector<double>> candidates)
{
    const int CHECK_SQUARE_SIZE = 10; //pixel length of half side
    const int H_MIN = 40 ;
    const int H_MAX = 75;
    const int S_MIN = 20;
    const int S_MAX = 255; 
    const double MIN_GREEN_REQUIRED = 30;

    int matchID = 0;
    int numMatch = 0;

    for(int i = candidates.size() - 1; i >= 0; i--)
    {
        //unpack the info for the individual candidate
        vector<double> testcandidate = candidates.at(i);

        //unpack the coordinates from the vector
        double x = testcandidate.at(1);
        double y = testcandidate.at(2);
        
        //skip if too close to the edge
        //this can be done better
        if(x >= mainfeed.cols - CHECK_SQUARE_SIZE) continue;  
        if(x <= CHECK_SQUARE_SIZE) continue;  
        if(y >= mainfeed.rows - CHECK_SQUARE_SIZE) continue;  
        if(y <= CHECK_SQUARE_SIZE) continue;  
        
        //create cv::Rect for area to check
        Rect greenrect(x - CHECK_SQUARE_SIZE,
                       y - CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE);

        //crop to a roi
        Mat greenroi = mainfeed(greenrect).clone();

        cvtColor(greenroi,greenroi,COLOR_BGR2HSV);
        //threshold color and saturation
        inRange(greenroi,Scalar(H_MIN,S_MIN,0),Scalar(H_MAX,S_MAX,255),greenroi);

        double totalgreen = countNonZero(greenroi);

        if(totalgreen > MIN_GREEN_REQUIRED)
        {
            //set the match id to the id of the iteration
            matchID = i;

            //increment the match counter
            numMatch++;
        }
    }

    //should change this to a throw and catch
    //we only want one match, not zero or many
    if(numMatch > 1) matchID = -1;
    if(numMatch == 0) matchID = -1;

    return matchID;
}



void im_proc::overlay_position(Mat *frame)
{
    //if statements mean it will only display if it has found a position
    //if statements use the first param of tuple    
    
    if(get<0>(Pos)){
        circle(*frame,Point(get<1>(Pos),get<2>(Pos)),20,Scalar(0,0,255),2);
    }
}

void im_proc::loadframe(Mat *frame)
{
    switch(FRAME_SOURCE){
        case 0:
            cap >> *frame;
            break;
        case 1:
            cap >> *frame;
            break;
        case 2:
            *frame = imread(FILENAME, CV_LOAD_IMAGE_COLOR);
            break;
    }

    if(! frame->data )                              // Check for invalid input
    {
        throw 4; 
    }
}
