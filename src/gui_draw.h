#ifndef GUI_DRAW_H
#define GUI_DRAW_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class gui_draw
{
    public:
        gui_draw();
        void draw_interface(cv::Mat frame);

    private:
        void create_trackbars();
        void display_image(const char* wtitle, cv::Mat frame);
};

#endif
