#ifndef IM_PROC_H
#define IM_PROC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class im_proc
{
    public:
        im_proc();
            
        cv::Mat getrawframe(const char* fname);
        cv::Mat getthreshold_frame(const char* fname); 
    private:
        cv::Mat loadframefile(const char* fname);
        cv::Mat threshold_frame(cv::Mat frame);
};
#endif
