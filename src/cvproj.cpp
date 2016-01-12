// Written by James Mare
// Goal is to manipulate the webcam stream


#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

char key; //stores key with waitKey

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

const int MORPH_ERODE_SIZE = 3;
const int MORPH_DILATE_SIZE = 8;

const int camID = 0; //0 for internal 1 for external

const string windowTitle1 = "Raw Cam Feed";
const string windowTitle2 = "HSV Feed";
const string windowTitle3 = "Thresholded Image";
const string windowTitle4 = "After morphOps";
const string windowTrackbars = "Trackbars";

int H_MIN = 45;
int H_MAX = 97;
int S_MIN = 91;
int S_MAX = 201;
int V_MIN = 63;
int V_MAX = 212;

void createTrackbars(){
	//create window for trackbars


    namedWindow(windowTrackbars, 1);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    createTrackbar( "H_MIN", windowTrackbars, &H_MIN, H_MAX);
    createTrackbar( "H_MAX", windowTrackbars, &H_MAX, H_MAX);
    createTrackbar( "S_MIN", windowTrackbars, &S_MIN, S_MAX);
    createTrackbar( "S_MAX", windowTrackbars, &S_MAX, S_MAX);
    createTrackbar( "V_MIN", windowTrackbars, &V_MIN, V_MAX);
    createTrackbar( "V_MAX", windowTrackbars, &V_MAX, V_MAX);


}


void morphOps(Mat &thresh){


	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(MORPH_ERODE_SIZE, MORPH_ERODE_SIZE));
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(MORPH_DILATE_SIZE, MORPH_DILATE_SIZE));

	erode(thresh,thresh,erodeElement);
//	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
	


}

int main(int argc, char* argv[])
{

    Mat feedCam;

    Mat feedHSV;

    Mat feedThreshold;
    
    createTrackbars();

    VideoCapture capture;

    capture.open(0);

    capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

    while(1)
    {

        capture.read(feedCam);
        
        cvtColor(feedCam, feedHSV, COLOR_BGR2HSV);
        
        
        inRange(feedHSV,Scalar(H_MIN,S_MIN,V_MIN),
            Scalar(H_MAX,S_MAX,V_MAX),feedThreshold);

        morphOps(feedThreshold);
        
        
        imshow(windowTitle1, feedCam);
        imshow(windowTitle2, feedHSV);
        imshow(windowTitle3, feedThreshold);
        key = cvWaitKey(10);     //Capture Keyboard stroke
        if (char(key) == 27){
            break;      //If you hit ESC key loop will break.
        }
    }

    return 0;

}

