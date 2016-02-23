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

## Parameter format

The parameter reading and writing system is extremely simple. Parameter files
must be text only. Even lines of parameter files are variable names and odd
lines are the integer values for those variables. A parameter file may have as
many or few parameters set as you like, and the order is not important.

The main reason for this system is to allow the parameters to be tuned on
a laptop and then moved to the embedded system, as the Pi does not have the
grunt to run X11 while proccessing images at an acceptable rate. (Trying to do
this will result in around 1 FPS with a big delay)

The mechanism to store last calibation values uses the same parameter format
and saves to lastcal.txt in the directory of the executable. To update just the
calibration values you can manually edit these values and use --readcal.

This parameter format is not robust and would be better replaced with something
like json.

## Program Description

Following a laser pointer is fairly challanging. There many visual
charactaristics that can be used to identify the laser. Color is the most
prominent to the human eye, and indoors color can very accuratly detect the
dot, but a color only approach requires very accurate tuning of the
thresholding values for each different lighting situation, as the way the
camera picks up the color changes drastically with the enviroment.

The image proccessing approach of this program is described below:

- The image is converted to greyscale and thresholded, it becomes a binary
  image, with all the very bright spots becoming white, and everything below
  the threshold becoming black.

- This image is dilated to reduce the number of objects that will be found.

- Each object that is bright and is within certain area thresholds becomes
  a 'candidate' and a structure of type "laserInfo" (defined in im_proc.h) is
  instantiated to hold information about it.

- Each candidate gets checked for color (amount of green pixels near it), size
  (how close is it to "areaExpected"), circularity (how much its area differs
  from the area of the smallest circle that can enclose the object).

- The candidate is assigned a score based on these charactaristics. The
  weighting of each charactaristic is set with variables and can be edited with
  trackbars.

- If two candidates are within certain distance limits of each other, they are
  identified as a 'pair'.

- If there are candidates over the minimum score threshold, the highest scoring
  candidate is chosen. Candidates identified as pairs get a score boost which
  is adjustable. This means that a pair of candidates will be chosen over
  a single candidate of the same score, but a low scoring pair (very common due
  to noise) will be ignored in favor of a high scoring single.

This approach is fairly successfull in coping with minor changes in lighting
and distance. Parameters that will work indoors will not be succcessfull
outdoors, but small changes in lighting and distance will be accomodated fairly
well, unlike the pure thresholding approach.
