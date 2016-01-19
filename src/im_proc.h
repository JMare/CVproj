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
        void threshold_frame(cv::Mat *frame, std::vector<int> *params);
        void morph_frame(cv::Mat *frame, std::vector<int> *params);
        void loadframe(cv::Mat *frame);
        void writeframe(cv::Mat *frame);

        std::tuple<bool, double, double> trackObject(cv::Mat *frame); 
        cv::Mat mainfeed;
        cv::Mat frame_proc1;
        cv::Mat frame_proc2;
        cv::Mat feed_conbri;
        cv::VideoCapture cap;
        void overlay_position(cv::Mat *frame);

        std::tuple<bool, double, double> Pos1;
        std::tuple<bool, double, double> Pos2;

};
#endif
