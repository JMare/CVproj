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

void im_proc::init_feed(int ID){
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
}

void im_proc::process_frame()
{
    loadframe(&mainfeed);
    
   //  mainfeed = darken_frame(&mainfeed);

    //clone is nessasary, assignment does not copy
    frame_proc1 = mainfeed.clone();
    frame_proc2 = mainfeed.clone();
    
    //proccessing 1
    threshold_frame(&frame_proc1, &imParams1);
    morph_frame(&frame_proc1, &imParams1);
    Pos1 = trackObject(&frame_proc1);

    //processing 2
    threshold_frame(&frame_proc2, &imParams2);
    morph_frame(&frame_proc2, &imParams2);
    Pos2 = trackObject(&frame_proc2);
    

    PosTemp = filterpositions(Pos1, Pos2); 
      
        if(get<0>(PosTemp)) PosMaster = PosTemp;

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

void im_proc::threshold_frame(Mat *frame, vector<int> *params)
{
    //convert color to hsv
    cvtColor(*frame, *frame, COLOR_BGR2HSV);

    //apply thresholding
    inRange(*frame,Scalar(
                params->at(0),   //H_MIN
                params->at(2),   //S_MIN
                params->at(4)),  //V_MIN

        Scalar(
                params->at(1),  //H_MAX
                params->at(3),  //S_MAX
                params->at(5)), //V_MAX
        *frame);
}

Mat im_proc::darken_frame(Mat *frame)
{
    Mat source = *frame;
    Mat RGBsmall, HSVsmall;
    int huesmall; 
    double greencount;
    Mat dark_out = Mat::zeros( frame->size(), frame->type() );
    int brightness = 7;
    int contrast = 1;
    int DARKEN_RADIUS = 2;
    int GREEN_PIXEL_MIN = 1;
    int DARKEN_HUE_MIN = 50;
    int DARKEN_HUE_MAX = 100;
    int yscanmin;
    int yscanmax;
    int xscanmin;
    int xscanmax;
    
    for( int y = 0; y < frame->rows; y++ )
    { 
        for( int x = 0; x < frame->cols; x++ )
        {
            greencount = 0;
            yscanmin = y - DARKEN_RADIUS;
            if(yscanmin < 0) yscanmin = 0;
            xscanmin = x - DARKEN_RADIUS;
            if(xscanmin < 0) xscanmin = 0;
            xscanmax = x + DARKEN_RADIUS;
            if(xscanmax > frame->cols) xscanmax = frame->cols;
            yscanmax = y + DARKEN_RADIUS;
            if(yscanmax > frame->rows) yscanmax = frame->rows;
            
            for( int yscan = yscanmin; yscan < yscanmax; yscan++)
            {
                for( int xscan = xscanmin; xscan < xscanmax; xscan++)
                {
                    RGBsmall = source(Rect(xscan,yscan,1,1));
                    cvtColor(RGBsmall, HSVsmall, CV_BGR2HSV);
                    Vec3b hsv = HSVsmall.at<Vec3b>(0,0);
                    huesmall = hsv.val[0]; 
                    if(DARKEN_HUE_MIN < huesmall < DARKEN_HUE_MAX)
                    {
                        greencount = greencount + 1;
                    }
                }
           }

            if(greencount < GREEN_PIXEL_MIN)
            {
                for(int c = 0; c < 3; c++)
                {
                    dark_out.at<Vec3b>(y,x)[c] =
                     saturate_cast<uchar>( contrast*( frame->at<Vec3b>(y,x)[c] ) - brightness );
                }
            } else
            {
                for(int c = 0; c < 3; c++)
                {
                    dark_out.at<Vec3b>(y,x)[c] =
                     saturate_cast<uchar>( contrast*( frame->at<Vec3b>(y,x)[c] ));
                }
            }
        }
    }
    
    return dark_out;
}


void im_proc::morph_frame(Mat *frame, vector<int> *params)
{
    //OpenCV will crash if it tries to erode or dilate with
    //a pixel size of zero, if statements stop this.
    
    if(params->at(6) != 0){
        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(params->at(6),params->at(6)));
        
        //for iterations erode
        for(int i=params->at(9); i>0; i--){
            erode(*frame,*frame,erodeElement);
        }
    }
    
    if(params->at(7) != 0){
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(params->at(7),params->at(7)));
        
        //for iterations dilate
        for(int i=params->at(8); i>0; i--){
            dilate(*frame,*frame,dilateElement);
        }
    }
}

tuple<bool, double, double> im_proc::trackObject(Mat *frame)
{

    Moments oMoments = moments(*frame);

    double posX;
    double posY;
    
    double dAreaMin = 20000;
    double dAreaMax = 50000;

    bool tracksuccess = false;

    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    //calculate the position of the  object
    posX = dM10 / dArea;
    posY = dM01 / dArea;        
    
    if(dArea >= dAreaMin || dArea <= dAreaMax)
    {
        if( posX >= 0 || posY >= 0)
        {
            tracksuccess = true;
        }
    }

    return make_tuple(tracksuccess, posX, posY);
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

    if (diffxy < MAX_DIST_ALLOWED || get<0>(pos1) == true || get<0>(pos2) == true) pos_valid = true; 
    
    
    //take average of x and y positions
    double posX = (get<1>(pos1) + get<1>(pos2)) / 2;
    double posY = (get<2>(pos1) + get<2>(pos2)) / 2;

    //return is valid and positions
    return make_tuple(pos_valid, posX, posY); 
}

/*tuple<bool, double, double> im_proc::trackObject(Mat *frame)
{
    Mat temp = frame->clone();
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    int MAX_NUM_OBJECTS = 10;
    int MIN_OBJECT_AREA = 0;
    int MAX_OBJECT_AREA = 300;

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
*/

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

