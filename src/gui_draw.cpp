#include "gui_draw.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

gui_draw::gui_draw()
{//constructor

}

void gui_draw::draw_interface(Mat frame)
{
    while(1){
        display_image("raw feed", frame); 

        int key;

        key = cvWaitKey(10);     //Capture Keyboard stroke
        if (char(key) == 27){
            break;      //If you hit ESC key loop will break.
        }
    }
}

void gui_draw::create_trackbars()
{

}

void gui_draw::display_image(const char* wtitle, Mat frame)
{
    imshow(wtitle, frame);
}

