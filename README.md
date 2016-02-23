# CVproj
OpenCV codebase with goal of tracking laser pointer

##Usage

CVproj can be run with multiple sources. Choose between webcam or video file

> ./cvrun.out --webcam {webcam id} 

> ./cvrun.out --vfile {file name}

Additional options are given below:

Enable Trackbars to adjust parameters. This option will also display the
thresholded image.
> --trackbar

Enable real time display of image with overlayed info.

> --gui

Enable command line output of frame rate and other info

> --debug

Load parameters from file

> --param-in somefile.txt

Write parameters to a file (Written every 10s, so wait a little while after you
make adjustments with trackbars to ensure they are saved)

> --param-out somefile.txt

Do a calibration sequence on startup, which sets the greyscale threshold and
the expected area of the target.

> --docal

Use the settings from the last calibration

> --readcal

(If neither --docal or --readcal is selected the program will either use the
defaults or, if the file specified by --param-in contains the calibration
values, it will use those)

Example usage on the embedded computer (command line only):

> ./cvproj --webcam 0 --param-in indoorparams.txt --param-out indoorparams.txt
> --debug --docal

Note that since --param-in and --param-out specify the same file, any changes
you make to the variables as the program runs will overwrite the variables you
loaded.

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

