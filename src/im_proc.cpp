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

        int numfound = get<1>(frame_info);
        cout << "frame inspected, found: " << numfound << " objects" << endl;
        
        double areafound = get<2>(frame_info);
        cout << "total area is: " << areafound;
        
        int numcandidates = get<0>(frame_info).size();
        cout << "number of candidates found is:" << numcandidates << endl;
        
        int matchID = check_candidates(get<0>(frame_info));
        
        if(matchID >= 0) laserfound = true;
        else imParams.at(0) = imParams.at(0) - 3;

        cout << "lowered threshold" << endl;
    } 
}

void im_proc::process_frame()
{
    loadframe(&mainfeed);
    
   //  mainfeed = darken_frame(&mainfeed);

    //clone is nessasary, assignment does not copy
    frame_proc1 = mainfeed.clone();
    
    //proccessing 1
    threshold_frame(&frame_proc1, &imParams);
    morph_frame(&frame_proc1, &imParams);
    Pos1 = trackObject(&frame_proc1);

    frame_info = inspect_frame(&frame_proc1);

   /* int numfound = get<1>(frame_info);
    cout << "frame inspected, found: " << numfound << " objects" << endl;
    
    double areafound = get<2>(frame_info);
    cout << "total area is: " << areafound;
    
    int numcandidates = get<0>(frame_info).size();
    cout << "number of candidates found is:" << numcandidates << endl;
*/
    PosTemp = filterpositions(Pos1, Pos2); 
    bool isvalid = get<0>(PosTemp); 

    if(isvalid) PosMaster = PosTemp;

}

Mat im_proc::get_frame_overlay()
{
    overlay_position(&mainfeed);
    return mainfeed;
}

Mat im_proc::get_frame_thresholded(int feedID)
{
    switch(feedID){
        case 1:
            return frame_proc1;
            break;
        case 2:
            return frame_proc2;
            break;
        default: 
            throw 2;
            break;
    }
}
//-------PRIVATE FUNCTRIONS----------------


void im_proc::threshold_frame(Mat *frame, vector<int> *params)
{
    vector<int> paramslocal = *params;

    cvtColor(*frame, *frame, CV_RGB2GRAY);
    threshold(*frame, *frame, paramslocal.at(0), paramslocal.at(1), THRESH_BINARY);
 //   adaptiveThreshold( *frame, *frame, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 10);

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



tuple<bool, double, double> im_proc::trackObject(Mat *frame)
{

    Mat temp = frame->clone();
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    
    double refArea = 0;
    bool objectFound = false;
    int MAX_NUM_OBJECTS = 20;
    int MIN_OBJECT_AREA = 50;
    int MAX_OBJECT_AREA = 500;

    double x, y;

    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {

                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;

                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
                    x = moment.m10/area;
                    y = moment.m01/area;
                    objectFound = true;
                    refArea = area;
                } else objectFound = false;
            }
        }   

    }
    return make_tuple(objectFound, x, y);
}



tuple< vector<vector<double>>, int, double> im_proc::inspect_frame(Mat *frame)
{

    Mat temp = frame->clone();

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
    int MAX_NUM_OBJECTS = 20;
    int MIN_OBJECT_AREA = 50;
    int MAX_OBJECT_AREA = 500;

    int numObjects = hierarchy.size();

    double x, y;
    double totalArea = countNonZero(*frame);
    
    vector<vector<double>> candidates;

    if (numObjects > 0 && numObjects < MAX_NUM_OBJECTS)
    {
        for (int index = 0; index >= 0; index = hierarchy[index][0]) 
        {

            Moments moment = moments((cv::Mat)contours[index]);
            double area = moment.m00;
            
            if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA)
            {
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
    int CHECK_SQUARE_SIZE = 10; //pixel length of half side
    int H_MIN = 40 ;
    int H_MAX = 75;
    int S_MIN = 20;
    int S_MAX = 255; 
    double MIN_GREEN_REQUIRED = 100;
    int matchID = 0;
    int numMatch = 0;

    for(int i = candidates.size() - 1; i >= 0; i--)
    {
        vector<double> testcandidate = candidates.at(i);
        double x = testcandidate.at(1);
        double y = testcandidate.at(2);
        
        Rect greenrect(x - CHECK_SQUARE_SIZE,
                       y - CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE);

        Mat greenroi = mainfeed(greenrect).clone();

        cvtColor(greenroi,greenroi,COLOR_BGR2HSV);
        inRange(greenroi,Scalar(H_MIN,S_MIN,0),Scalar(H_MAX,S_MAX,255),greenroi);

        double totalgreen = countNonZero(greenroi);

        if(totalgreen > MIN_GREEN_REQUIRED)
        {
            matchID = i;
            numMatch++;
        }
    }
    cout << "found " << numMatch << " green objects" << endl;

    if(numMatch > 1) matchID = -1;
    if(numMatch == 0) matchID = -1;

    return matchID;
}



void im_proc::overlay_position(Mat *frame)
{
    //if statements mean it will only display if it has found a position
    //if statements use the first param of tuple    
    
    if(get<0>(Pos1)){
        circle(*frame,Point(get<1>(Pos1),get<2>(Pos1)),20,Scalar(0,0,255),2);
    }

    if(get<0>(Pos2)){
        circle(*frame,Point(get<1>(Pos2),get<2>(Pos2)),20,Scalar(0,255,0),2);
    }

    if(get<0>(PosMaster)){
        circle(*frame,Point(get<1>(PosMaster),get<2>(PosMaster)),30,Scalar(255,0,0),4);
    }
}

tuple<bool, double, double> im_proc::filterpositions(tuple<bool, double, double> pos1, tuple<bool, double, double> pos2)
{
    //this function takes in the reported positions from
    //each proccessing stream and decides whether they are
    //close enough to be a match
    

    bool pos_valid = false;
    int MAX_DIST_ALLOWED = 10;

    //Calculate dist between reported positions
    double diffx = abs(get<1>(pos1) - get<1>(pos2));
    double diffy = abs(get<2>(pos1) - get<2>(pos2));

    //calculate hypotenuse
    double diffxy = sqrt( (diffx * diffx) + (diffy * diffy));

    if (diffxy <= MAX_DIST_ALLOWED && get<0>(pos1) && get<0>(pos2)) pos_valid = true; 
    
    //take average of x and y positions
    double posX = (get<1>(pos1) + get<1>(pos2)) / 2;
    double posY = (get<2>(pos1) + get<2>(pos2)) / 2;

    //return is valid and positions
    return make_tuple(pos_valid, posX, posY); 

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
