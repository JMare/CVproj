// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.h - header file for im_proc class

#ifndef IM_PROC_H
#define IM_PROC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <tuple>

struct laserInfo
{
    bool isMatch;
    float x;
    float y;
    float area;
    float minCircleArea;
    int colorCount;
    int matchScore;
   
    int pairID;
    float pairDist;    

    //structrure constructor
    laserInfo(){
        isMatch = false;
        colorCount = 0;
        matchScore = 0;
        pairID = -1;
    }
};

class im_proc
{
    public:
        im_proc();
           
        void init_feed(int ID);
        laserInfo process_frame(); 

        cv::Mat get_frame_overlay();
        cv::Mat get_frame_thresholded();

    private:
        //FUNCTIONS
        void threshold_frame(cv::Mat *frame);
        void morph_frame(cv::Mat *frame);
        void loadframe(cv::Mat *frame);
        void overlay_position(cv::Mat *frame,
                   std::vector<laserInfo>* laserContainerPointer,
                   laserInfo masterPosition);
        
        void check_candidates(std::vector<laserInfo>* laserContainerPointer);

        void checkObjectColor(int CHECK_SQUARE_SIZE,
                               int H_MIN,
                               int H_MAX,
                               int S_MIN,
                               int S_MAX,
                               std::vector<laserInfo>* laserContainerPointer);

        void calcObjectScores(std::vector<laserInfo>* laserContainerPointer, int MIN_GREEN_REQUIRED);

        laserInfo calcMasterPosition(std::vector<laserInfo>* laserContainerPointer);
        std::vector<laserInfo>* inspect_frame(cv::Mat *frame, std::vector<laserInfo>* laserContainerPointer);

        
        //VARIABLES AND OBJECTS
        cv::Mat mainfeed;
        cv::Mat frame_proc;
        
        cv::VideoCapture cap;

        const int MIN_LASER_DIST = 10;
        const int MAX_LASER_DIST = 200;

};


#endif
