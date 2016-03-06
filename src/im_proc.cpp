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
#include <math.h>

//----NAMESPACES----------------
using namespace std;
using namespace cv;

const int STEP_UP = 5;
const int STEP_DOWN = 2;
const int STEP_FINAL = 4;
const int HISTORY_LENGTH = 5;
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
        //cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
        //
        //cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
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
}   

void im_proc::calibrateImage()
{
    cout << "Please ensure the laser is not in the frame." << endl;

    cout << "Press enter to continue." << endl;
    system("read");

    //discard first 60 frames to ensure webcam exposure has settled
    for( int i = 0; i <= 30; i++)
    {
        loadframe(&mainfeed);
    }

    vector<laserInfo> laserContainer;
    bool emptyframe = false;

    while (!emptyframe)
    {
        loadframe(&mainfeed);
        Mat frame_proc = mainfeed.clone();

        threshold_frame(&frame_proc);
        morph_frame(&frame_proc);
        
        inspect_frame(&frame_proc, &laserContainer);
            
        double areafound = 0;

        for(int  i = 0; i < laserContainer.size(); i++){
            areaFound = areaFound + (laserContainer.at(i)).area;
        }
        
        if(areafound == 0) emptyframe = true;

        gParams.greyThreshMin = gParams.greyThreshMin + STEP_UP;

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

    while(!laserfound && gParams.greyThreshMin > 0)
    {

        loadframe(&mainfeed);

        //clone is nessasary, assignment does not copy
        frame_proc = mainfeed.clone();

        threshold_frame(&frame_proc);
        morph_frame(&frame_proc);

        inspect_frame(&frame_proc, &laserContainer);

        check_candidates(&laserContainer);

        laserInfo masterPosition = calcMasterPosition(&laserContainer);
        
        if(masterPosition.isMatch == true) 
        {
            laserfound = true;
            gParams.calcAreaExpected = masterPosition.area;
        }
        else 
        {
            gParams.greyThreshMin = gParams.greyThreshMin - STEP_DOWN;
            cout << "lowered threshold" << endl;
        }
    }

    //then after everything go down a bit more to make sure
    gParams.greyThreshMin = gParams.greyThreshMin - STEP_FINAL;
    
}

laserInfo im_proc::process_frame()
{
   
    vector<laserInfo> laserContainer;

    bool historyCheck = true;

    double xSmooth;
    double ySmooth;

    loadframe(&mainfeed);

    //clone is nessasary, assignment does not copy
    frame_proc = mainfeed.clone();

    threshold_frame(&frame_proc);
    morph_frame(&frame_proc);

    inspect_frame(&frame_proc, &laserContainer);

    check_candidates(&laserContainer);

    laserInfo masterPosition = calcMasterPosition(&laserContainer);

    overlay_position(&mainfeed, &laserContainer, masterPosition);

    //The below commented section is for position smoothing. 
    //the program seems to run better without it, given the low frame
    //rate on the embeddded
   /* 
    //update last positions
    PosHistory.push_back(masterPosition); 
    if(PosHistory.size() > HISTORY_LENGTH)
    {
        PosHistory.erase(PosHistory.begin());
    }

    if(masterPosition))
    {
        vector<float> xHistory, yHistory;
        for(int i = 0; i < PosHistory.size(); i++){
            if(get<1>(PosHistory.at(i)) != -1){
                xHistory.push_back(get<1>(PosHistory.at(i)));    
                yHistory.push_back(get<2>(PosHistory.at(i)));    
            }
        }

        float xsum = std::accumulate(xHistory.begin(), xHistory.end(), 0.0);
        float xSmooth = xsum / xHistory.size();

        float ysum = std::accumulate(yHistory.begin(), yHistory.end(), 0.0);
        float ySmooth = ysum / yHistory.size();

        Posmaster = make_tuple(true, xSmooth, ySmooth);
    } else Posmaster = make_tuple(false, -1, -1);
*/
    return masterPosition;
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


void im_proc::threshold_frame(Mat *frame)
{
    cvtColor(*frame, *frame, CV_RGB2GRAY);
    threshold(*frame, *frame, gParams.greyThreshMin, gParams.greyThreshMax, THRESH_BINARY);
}


void im_proc::morph_frame(Mat *frame)
{
    //OpenCV will crash if it tries to erode or dilate with
    //a pixel size of zero, if statements stop this.
    
    if(gParams.greyErodePix != 0){
        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(gParams.greyErodePix,gParams.greyErodePix));
        
        //for iterations erode
        for(int i = gParams.greyErodeIterations; i>0; i--){
            erode(*frame,*frame,erodeElement);
        }
    }
    
    if(gParams.greyDilatePix != 0){
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(gParams.greyDilatePix,gParams.greyDilatePix));
        
        //for iterations dilate
        for(int i = gParams.greyDilateIterations; i>0; i--){
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
    
    int MAX_NUM_OBJECTS = gParams.candMaxNumObjects; 
    int MIN_OBJECT_AREA = gParams.candMinObjectArea;
    int MAX_OBJECT_AREA = gParams.candMaxObjectArea; 

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
                //find the min enclosing circle for the object
                Point2f center;
                float radius;
                minEnclosingCircle(contours[index], center, radius);

                //if area matches, calculate position and
                //pack it into the vector of candidates
                x = moment.m10/area;
                y = moment.m01/area;
                
                //initialize object and assign values
                laserInfo laserTemp;
                laserTemp.x = x;
                laserTemp.y = y;
                laserTemp.area = area;
                laserTemp.minCircleArea = M_PI*radius*radius;

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
    int CHECK_SQUARE_SIZE     = gParams.checkSquareSize;
    int H_MIN                 = gParams.checkHMin; 
    int H_MAX                 = gParams.checkHMax; 
    int S_MIN                 = gParams.checkSMin; 
    int S_MAX                 = gParams.checkSMax; 
    int MIN_GREEN_REQUIRED    = gParams.checkMinGreen; 
   
    //call colorcheck to count and apply greenness to objects
    checkObjectColor(CHECK_SQUARE_SIZE,
                H_MIN, H_MAX, S_MIN, S_MAX, 
                laserContainerPointer);

    //call calcObjectScores to balance the different methods of detection
    calcObjectScores(laserContainerPointer, MIN_GREEN_REQUIRED);
}

void im_proc::calcObjectScores(vector<laserInfo>* laserContainerPointer, int MIN_GREEN_REQUIRED)
{

    //Score weighting variables
    const int scorePercentColor = gParams.calcScorePercentColor;
    const int scorePercentColorExtra = gParams.calcScorePercentColorExtra;
    const int scorePercentSize = gParams.calcScorePercentSize;
    const int scorePercentCircle = gParams.calcScorePercentCircle;

    const int AREA_EXPECTED = gParams.calcAreaExpected;
    const int AREA_MAX_DIFF = gParams.calcAreaMaxDiff;

    const int MIN_PAIR_DIST = gParams.calcMinPairDist;
    const int MAX_PAIR_DIST = gParams.calcMaxPairDist;

    //how much green will result in all the extra points
    int MAX_GREEN_EXTRA = MIN_GREEN_REQUIRED * 3;

    float extraGreenPercent;

    //iterate through all the things
    for (int i = 0; i < laserContainerPointer->size(); i++)
    {
        laserInfo *laserToCheck = &laserContainerPointer->at(i);
        
        //Look at the greenness and assign score
        if(laserToCheck->colorCount > MIN_GREEN_REQUIRED){
            //If green is over the threshold give a score
            laserToCheck->matchScore = laserToCheck->matchScore + scorePercentColor;

            //Give extra points for how much the green is over the threshold, up to MAX_GREEN_EXTRA
            if(laserToCheck->colorCount > MAX_GREEN_EXTRA) extraGreenPercent = 1;
            else extraGreenPercent = float(laserToCheck->colorCount) / float(MAX_GREEN_EXTRA);
            float extraGreen = extraGreenPercent * scorePercentColorExtra ;
            laserToCheck->matchScore = laserToCheck->matchScore + extraGreen;
        }

        //look at the sizeness and assign score
        //Calculate difference
        float areaDiff = abs(laserToCheck->area - AREA_EXPECTED);  
        float areaDiffDec;
        //Calculate decimal difference, with cap at 1
        if(areaDiff < AREA_MAX_DIFF) areaDiffDec = areaDiff / AREA_MAX_DIFF;
        else areaDiffDec = 1;
       
        //subtract from 1, this means the lower the score the bigger the multipler
        float areaZerotoOne = abs(1 - areaDiffDec);

        //apply score based on how close the area is to the expected
        laserToCheck->matchScore = laserToCheck->matchScore + scorePercentSize * areaZerotoOne;


        //Look for circleness and apply score
        float circleAreaDec = laserToCheck->area / laserToCheck->minCircleArea; 
        laserToCheck->matchScore = laserToCheck->matchScore + scorePercentCircle * circleAreaDec;

        //Check for pairs close to each other and assign pairID
        if(laserToCheck->pairID == -1)
        {
            //If no pair has been found yet, iterate though others
            for(int n = 0; n < laserContainerPointer->size(); n++)
            {
                if(n != i) //dont check self pairs
                {
                    float diffx = abs(laserToCheck->x - (laserContainerPointer->at(n)).x);
                    float diffy = abs(laserToCheck->y - (laserContainerPointer->at(n)).y);
                    float totDiff = sqrt(diffx*diffx + diffy*diffy);
                    
                    if(totDiff < MAX_PAIR_DIST && totDiff > MIN_PAIR_DIST)
                    {
                        //pair has been found, set pair id and totDiff on both 
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


laserInfo im_proc::calcMasterPosition(vector<laserInfo>* laserContainerPointer)
{
    const int MIN_SCORE_NON_PAIR = gParams.masterMinScoreNonPair;
    const int MIN_SCORE_PAIR = gParams.masterMinScorePair;
    const int PAIR_SCORE_BOOST = gParams.masterPairScoreBoost;
    
    int bestPairID = -1;
    int bestSingleID = -1;
    int pairScore = 0;
    int bestSingleScore = 0;
    int bestPairScore = 0;
    
    for(int i = 0; i < laserContainerPointer->size(); i++)
    {
        laserInfo *laserToCheck = &laserContainerPointer->at(i);

        if(laserToCheck->pairID == -1){ //if not pair
            if(laserToCheck->matchScore > MIN_SCORE_NON_PAIR){
                if(laserToCheck->matchScore > bestSingleScore){
                    bestSingleID = i;
                    bestSingleScore = laserToCheck->matchScore;
                }
            }
        } else { //is pair
            pairScore = ((laserToCheck->matchScore + (laserContainerPointer->at(laserToCheck->pairID)).matchScore) /2) + PAIR_SCORE_BOOST ;
            if(pairScore > MIN_SCORE_PAIR){
                if(pairScore > bestPairScore)
                {
                    bestPairID = i;
                    bestPairScore = pairScore;
                }
            }
        }
    }

    if(bestPairID == -1 && bestSingleID == -1){
        laserInfo blankreturn;
        return blankreturn;
    }
    else if(bestPairID == -1 && bestSingleID != -1){
        (laserContainerPointer->at(bestSingleID)).isMatch = true;
        return laserContainerPointer->at(bestSingleID); 
    } else if(bestPairID != -1 && bestSingleID == -1){
        (laserContainerPointer->at(bestPairID)).isMatch = true;
        return laserContainerPointer->at(bestPairID); 
    } else {
        if((laserContainerPointer->at(bestSingleID)).matchScore > pairScore){
            (laserContainerPointer->at(bestSingleID)).isMatch = true;
            return laserContainerPointer->at(bestSingleID);
        } else {
            (laserContainerPointer->at(bestPairID)).isMatch = true;
            return laserContainerPointer->at(bestPairID);
        }
    }
    
} //calcMasterPosition

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
           laserInfo masterPosition)
{
    for(int i = 0; i < laserContainerPointer->size(); i++){
        laserInfo* laserToDraw = &laserContainerPointer->at(i);

        //print dot on each candidate
        circle(*frame,Point(laserToDraw->x,laserToDraw->y),2,Scalar(0,0,255),3);

        //print score next to each candidate
        std::ostringstream str;
        str << laserToDraw->matchScore;;
        putText(*frame,str.str(), Point(laserToDraw->x + 7,laserToDraw->y + 7), CV_FONT_HERSHEY_PLAIN, 1,Scalar(0,0,250));

        if(laserToDraw->pairID != -1)
        {
            float xtarget = (laserContainerPointer->at(laserToDraw->pairID)).x;
            float ytarget = (laserContainerPointer->at(laserToDraw->pairID)).y;
            
            line(*frame,Point(laserToDraw->x, laserToDraw->y),Point(xtarget,ytarget),Scalar(0,0,255),1,CV_AA, 0);
        }
    } //end for

    //If master position calculated put a circle around it
    if(masterPosition.isMatch == true)
    {
        circle(*frame,Point(masterPosition.x,masterPosition.y),5,Scalar(0,255,0),3);
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
