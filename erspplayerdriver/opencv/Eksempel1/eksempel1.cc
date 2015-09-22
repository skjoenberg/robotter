#include "highgui.h"

int main(int argc, char *argv[]) 
{
    // Get an OpenCV camera handle
    CvCapture *cam = cvCaptureFromCAM(0);

    // Initialise the GUI
    const char *window = "Eksempel 1";
    cvNamedWindow(window, CV_WINDOW_AUTOSIZE);

    // The main loop    
    while (cvWaitKey(4) == -1) {
        IplImage *im = cvQueryFrame(cam);
        cvShowImage(window, im);
    }

    // Clean up
    cvDestroyAllWindows();
    cvReleaseCapture(&cam);
    
    return 0;
}
