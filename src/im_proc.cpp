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

const int STEP_UP = 5;
const int STEP_DOWN = 2;
const int STEP_FINAL = 4;
const int HISTORY_LENGTH = 10;
const int LOST_FIX_FRAME = 5;
vector<vector<double>> lastcandidates;
vector<tuple<bool, double, double>> PosHistory;

int lostFixCount = 0;
double areaFound;
double xFound;
double yFound;

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
    /*
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

        imParams.at(0) = imParams.at(0) + STEP_UP;

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

    //start the loop lowering threshold while looking for laser
    
    while(!laserfound && imParams.at(0) > 0)
    {
        loadframe(&mainfeed);
        Mat frame_proc = mainfeed.clone();
        threshold_frame(&frame_proc, &imParams);
        morph_frame(&frame_proc, &imParams);
        frame_info = inspect_frame(&frame_proc);
        
        tuple<bool, double, double>  matchFound = check_candidates(get<0>(frame_info));
        
        if(get<0>(matchFound) == true) 
        {
            laserfound = true;
            //vector<vector<double>> candidatesFound = get<0>(frame_info);
            //vector<double> matchFound = candidatesFound.at(matchID);
            //areaFound = matchFound.at(0);
            
            //set min and max area thresholds based on 
            //inspect_image_params.at(1) = areaFound - 250;
            //inspect_image_params.at(2) = areaFound + 400;
            //int areaFound = matchFound.at(0);
        }
        else 
        {
            imParams.at(0) = imParams.at(0) - STEP_DOWN;
            cout << "lowered threshold" << endl;
        }
    }

    //then after everything go down a bit more to make sure
    imParams.at(0) = imParams.at(0) - STEP_FINAL;
    */

    //hardcoded hack
    imParams.at(0) = 230;
}

tuple<bool, float, float> im_proc::process_frame()
{
   
    vector<laserInfo> laserContainer;

    bool historyCheck = true;

    double xSmooth;
    double ySmooth;

    loadframe(&mainfeed);

    //clone is nessasary, assignment does not copy
    frame_proc = mainfeed.clone();

    threshold_frame(&frame_proc, &imParams);
    morph_frame(&frame_proc, &imParams);

    inspect_frame(&frame_proc, &laserContainer);

    check_candidates(&laserContainer);

    tuple<bool, float, float> masterPosition = calcMasterPosition(&laserContainer);

    overlay_position(&mainfeed, &laserContainer, masterPosition);

    return masterPosition;
    
/*    for(int i = 0; i < PosHistory.size(); i++)
    {
        tuple<bool, double, double> PosCheck = PosHistory.at(i);
        if(!get<0>(PosCheck)) historyCheck = false;
    }

    if(get<0>(Pos) && historyCheck)
    {
        const double filteramount = 0.4;
        double xLast = get<1>(Posmaster);
        double yLast = get<2>(Posmaster);
        double xNow = get<1>(Pos);
        double yNow = get<2>(Pos);
        double xChange = xNow -xLast;
        double yChange = yNow - yLast;

        if(xLast != -1){
            xSmooth = xLast + (filteramount * xChange);
            ySmooth = yLast + (filteramount * yChange);
        } else {
            xSmooth = xNow;
            ySmooth = yNow;
        }
        Posmaster = make_tuple(true, xSmooth, ySmooth);
    } else{
        lostFixCount++;
        if(lostFixCount > LOST_FIX_FRAME){
            Posmaster = make_tuple(false, -1, -1);
            lostFixCount = 0;
        }
    }

    //update last positions
    PosHistory.push_back(Pos); 
    if(PosHistory.size() > HISTORY_LENGTH)
    {
        PosHistory.erase(PosHistory.begin());
    }*/
}

Mat im_proc::get_frame_overlay()
{
    return mainfeed;
}

Mat im_proc::get_frame_thresholded()
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


vector<laserInfo>* im_proc::inspect_frame(Mat *frame, vector<laserInfo>* laserContainerPointer)
{
    Mat temp = frame->clone();

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
    int MAX_NUM_OBJECTS = inspect_image_params.at(0); 
    int MIN_OBJECT_AREA = inspect_image_params.at(1); 
    int MAX_OBJECT_AREA = inspect_image_params.at(2); 

    int numObjects = hierarchy.size();

    float x, y;

    double totalArea = countNonZero(*frame);

    if (numObjects > 0 && numObjects < MAX_NUM_OBJECTS)
    {
        for (int index = 0; index >= 0; index = hierarchy[index][0]) 
        {
            //for each object

            Moments moment = moments((cv::Mat)contours[index]);
            float area = moment.m00;
            
            if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA)
            {
                //if area matches, calculate position and
                //pack it into the vector of candidates
                x = moment.m10/area;
                y = moment.m01/area;
                
                //initialize object and assign values
                laserInfo laserTemp;
                laserTemp.x = x;
                laserTemp.y = y;
                laserTemp.area = area;

                //pack into container
                laserContainerPointer->push_back(laserTemp);
            } 
        } 
    } else if (numObjects > MAX_NUM_OBJECTS){
        //noisy filter/too many objects throw
       // throw 41;
    }
}
            
void im_proc::check_candidates(vector<laserInfo>* laserContainerPointer)
{
    int CHECK_SQUARE_SIZE     = check_candidates_params.at(0);
    int H_MIN                 = check_candidates_params.at(1);
    int H_MAX                 = check_candidates_params.at(2);
    int S_MIN                 = check_candidates_params.at(3);
    int S_MAX                 = check_candidates_params.at(4);
    int MIN_GREEN_REQUIRED    = check_candidates_params.at(5);
   
    //call colorcheck to count and apply greenness to objects
    checkObjectColor(CHECK_SQUARE_SIZE,
                H_MIN, H_MAX, S_MIN, S_MAX, 
                laserContainerPointer);

    //call calcObjectScores to balance the different methods of detection
    calcObjectScores(laserContainerPointer, MIN_GREEN_REQUIRED);

    /*
    if(greenMatches.size() == 2){
        //if there are two green objects, work out how close they are
        vector<double> match1 = greenMatches.at(0);
        vector<double> match2 = greenMatches.at(1);
        double x1 = match1.at(0);
        double x2 = match2.at(0);
        double y1 = match1.at(1);
        double y2 = match2.at(1);

        double deltax = abs(x1 - x2);
        double deltay = abs(y1 - y2);
        double dist = sqrt(deltax*deltax + deltay*deltay);
        
        if(dist > MIN_LASER_DIST && dist < MAX_LASER_DIST)
        {
            xreturn = (x1 + x2) / 2;
            yreturn = (y1 + y2) / 2;
            identifiedPair = true;
        }
    }
    */


}

void im_proc::calcObjectScores(vector<laserInfo>* laserContainerPointer, int MIN_GREEN_REQUIRED)
{

    const int scorePercentColor = 15;
    const int scorePercentColorExtra = 35;
    const int scorePercentSize = 50;

    const int AREA_EXPECTED = 120;
    const int AREA_MAX_DIFF = 200;

    const int MIN_PAIR_DIST = 0;
    const int MAX_PAIR_DIST = 100;

    //how much green will result in all the extra points
    int MAX_GREEN_EXTRA = MIN_GREEN_REQUIRED * 20;
    float extraGreenPercent;

    //iterate through all the things
    for (int i = 0; i < laserContainerPointer->size(); i++)
    {
        laserInfo *laserToCheck = &laserContainerPointer->at(i);
        
        //Look at the greenness and assign score
        if(laserToCheck->colorCount > MIN_GREEN_REQUIRED){
            laserToCheck->matchScore = laserToCheck->matchScore + scorePercentColor;

            if(laserToCheck->colorCount > MAX_GREEN_EXTRA) extraGreenPercent = 1;
            else extraGreenPercent = float(laserToCheck->colorCount) / float(MAX_GREEN_EXTRA);

            float extraGreen = extraGreenPercent * scorePercentColorExtra ;
            laserToCheck->matchScore = laserToCheck->matchScore + extraGreen;
        }

        //look at the sizeness and assign score
        float areaDiff = abs(laserToCheck->area - AREA_EXPECTED);  
        float areaDiffDec;
        if(areaDiff < AREA_MAX_DIFF) areaDiffDec = areaDiff / AREA_MAX_DIFF;
        else areaDiffDec = 1;
        float areaZerotoOne = abs(1 - areaDiffDec);
        laserToCheck->matchScore = laserToCheck->matchScore + scorePercentSize * areaZerotoOne;


        //Check for pairs close to each other and assign pairID
        if(laserToCheck->pairID == -1)
        {
            for(int n = 0; n < laserContainerPointer->size(); n++)
            {
                if(n != i)
                {
                    float diffx = abs(laserToCheck->x - (laserContainerPointer->at(n)).x);
                    float diffy = abs(laserToCheck->y - (laserContainerPointer->at(n)).y);
                    float totDiff = sqrt(diffx*diffx + diffy*diffy);
                    
                    if(totDiff < MAX_PAIR_DIST && totDiff > MIN_PAIR_DIST)
                    {
                        laserToCheck->pairID = n;
                        laserToCheck->pairDist = totDiff;
                        (laserContainerPointer->at(n)).pairID = i;
                        (laserContainerPointer->at(n)).pairDist = totDiff;
                    }
                }
            }
        }

    }
}


tuple<bool, float, float> im_proc::calcMasterPosition(vector<laserInfo>* laserContainerPointer){
    
    const int MIN_SCORE_NON_PAIR = 50;
    const int MIN_SCORE_PAIR = 25;

    return make_tuple(true, 100, 200);
}

void im_proc::checkObjectColor(int CHECK_SQUARE_SIZE,
                               int H_MIN,
                               int H_MAX,
                               int S_MIN,
                               int S_MAX,
                               vector<laserInfo>* laserContainerPointer)
{
    for (int i = 0; i < laserContainerPointer->size(); i++)
    {
        laserInfo* laserToCheck = &laserContainerPointer->at(i);

        //skip if too close to the edge
        //this can be done better
        if(laserToCheck->x >= mainfeed.cols - CHECK_SQUARE_SIZE) return;  
        if(laserToCheck->x <= CHECK_SQUARE_SIZE) return;  
        if(laserToCheck->y >= mainfeed.rows - CHECK_SQUARE_SIZE) return;  
        if(laserToCheck->y <= CHECK_SQUARE_SIZE) return;  
        
        //create cv::Rect for area to check
        Rect greenrect(laserToCheck->x - CHECK_SQUARE_SIZE,
                       laserToCheck->y - CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE,
                       2 * CHECK_SQUARE_SIZE);

        //crop to a roi
        Mat greenroi = mainfeed(greenrect).clone();

        //convert to hsv
        cvtColor(greenroi,greenroi,COLOR_BGR2HSV);

        //threshold color and saturation
        inRange(greenroi,Scalar(H_MIN,S_MIN,0),Scalar(H_MAX,S_MAX,255),greenroi);
        
        //set parameter of structure to the green we have counted
        laserToCheck->colorCount = countNonZero(greenroi);
    }
}

void im_proc::overlay_position(cv::Mat *frame,
           std::vector<laserInfo>* laserContainerPointer,
           std::tuple<bool, float, float> masterPosition)
{
    for(int i = 0; i < laserContainerPointer->size(); i++){
        laserInfo* laserToDraw = &laserContainerPointer->at(i);

        //print dot on each candidate
        circle(*frame,Point(laserToDraw->x,laserToDraw->y),2,Scalar(0,0,255),3);

        //print score next to each candidate
        std::ostringstream str;
        str << laserToDraw->matchScore;;
        putText(*frame,str.str(), Point(laserToDraw->x + 5,laserToDraw->y + 5), CV_FONT_HERSHEY_PLAIN, 1,Scalar(0,0,250));

        if(laserToDraw->pairID != -1)
        {
            float xtarget = (laserContainerPointer->at(laserToDraw->pairID)).x;
            float ytarget = (laserContainerPointer->at(laserToDraw->pairID)).y;
            
            line(*frame,Point(laserToDraw->x, laserToDraw->y),Point(xtarget,ytarget),Scalar(0,0,255),1,CV_AA, 0);
        }
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
