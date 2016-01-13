// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.h - header file for im_proc class

#ifndef IM_PROC_H
#define IM_PROC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class im_proc
{
    public:
        im_proc(int ID);
            
        cv::Mat getrawframe();
        cv::Mat getprocessed_frame(); 
    private:
        cv::Mat loadframefile(const char* fname);
        cv::Mat threshold_frame(cv::Mat frame);
        cv::Mat morph_frame(cv::Mat frame);
        cv::Mat loadframewebcam();
        void trackObject(cv::Mat frame); 
        cv::VideoCapture cap;
        cv::Mat overlay_position(cv::Mat frame);
};
#endif
