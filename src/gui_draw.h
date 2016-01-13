// Written by James Mare
// OpenCV program with goal of following a laser pointer
// gui_draw.h - class definitions for gui drawing 

#ifndef GUI_DRAW_H
#define GUI_DRAW_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class gui_draw
{
    public:
        gui_draw();
        void draw_interface();

    private:
        void create_trackbars();
        void display_image(const char* wtitle, cv::Mat frame);
};

#endif
