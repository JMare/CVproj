# CVproj
OpenCV codebase with goal of tracking laser pointer

##Usage

CVproj can be run with multiple sources. Choose between webcam, video file or
image file.

> ./cvrun.out --webcam {webcam id} 

> ./cvrun.out --vfile {file name}

> ./cvrun.out --ifile {file name}

Additionally, append the following to enable trackbars to modify thresholding
and morphalogical variables. CVproj uses image processing streams, so use the
following syntax to select which one you want to adjust with the trackbars.

> --trackbar {stream ID} 

## Notes 

If you instantiate a cv::Mat and then return it without assigning it to
anything you will get a segfault

## Program Description

The objective is to follow a laser pointer. The current approach is to take
a frame, make two copies of it, apply thresholding, morphalogical operations
and object detection to both, Then use the positions to decide whether we have
found the laser pointer.

For example, currently thread 1 looks for the green ring around the laser dot,
and thread 2 looks for the bright spot in the middle. If these two streams
return very similar positions, we can be fairly confident we have found our
mark.

