#ifndef IM_PROC_H
#define IM_PROC_H

#include <cv.h>
#include <highgui.h>

class im_proc
{
    public:
       im_proc();
        
        cv::Mat getfeed();
        
    private:
        cv::VideoCapture capture;
};
#endif
