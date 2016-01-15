// Written by James Mare
// OpenCV program with goal of following a laser pointer
// im_proc.h - header file for im_proc class

#ifndef IM_PROC_H
#define IM_PROC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>

class im_proc
{
    public:
        im_proc(int ID);
            
        cv::Mat process_frame(); 

    private:
        void threshold_frame(cv::Mat *frame);
        void morph_frame(cv::Mat *frame);
        void loadframe(cv::Mat *frame);

        std::vector<double> trackObject(cv::Mat *frame); 

        cv::VideoCapture cap;
        void overlay_position(cv::Mat *frame);
};
#endif
