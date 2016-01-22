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

class im_proc
{
    public:
        im_proc();
           
        void init_feed(int ID);
        void process_frame(); 

        cv::Mat get_frame_overlay();
        cv::Mat get_frame_thresholded(int feedID);
        

    private:
        //FUNCTIONS
        void threshold_frame(cv::Mat *frame, std::vector<int> *params);
        void morph_frame(cv::Mat *frame, std::vector<int> *params);
        void loadframe(cv::Mat *frame);
        void overlay_position(cv::Mat *frame);
        
        int check_candidates(std::vector<std::vector<double>>);
        std::tuple< std::vector<std::vector<double>>, int, double> inspect_frame(cv::Mat *frame);       

        
        //VARIABLES AND OBJECTS
        cv::Mat mainfeed;
        cv::Mat frame_proc;
        
        cv::VideoCapture cap;

        std::tuple<bool, double, double> Pos;
        std::tuple< std::vector<std::vector<double>>, int, double> frame_info;       
};

#endif
