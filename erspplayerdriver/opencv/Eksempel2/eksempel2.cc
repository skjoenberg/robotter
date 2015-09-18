#include "cxcore.h"
#include "cv.h"
#include "highgui.h"

int main(int argc, char *argv[]) 
{
    // Get an OpenCV camera handle
    CvCapture *cam = cvCaptureFromCAM(-1);
    cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_WIDTH, 640);
    cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_HEIGHT, 480);

    // Initialise the GUI
    const char *window = "Eksempel 2";
    cvNamedWindow(window, CV_WINDOW_AUTOSIZE);

    // Allocate an 640x480 gray scale image for the gray scale image and 
    // the edge image.
    // This assumes that the camera returns 640x480 images.
    const CvSize size = cvSize(640, 480);
    IplImage *gray  = cvCreateImage(size, IPL_DEPTH_8U, 1);
    IplImage *edges = cvCreateImage(size, IPL_DEPTH_8U, 1);
    
    // The main loop
    while (cvWaitKey(4) == -1) {
        IplImage *im = cvQueryFrame(cam);
        cvCvtColor(im, gray, CV_BGR2GRAY); // Convert color image to gray scale
        cvCanny(gray, edges, 1000, 750, 5); // Find edges
        cvShowImage(window, edges);
    }
    cvSaveImage("edges.png", edges); // Save the last edge image

    // Clean up
    cvDestroyAllWindows();
    cvReleaseCapture(&cam);
    cvReleaseImage(&gray);
    cvReleaseImage(&edges);
    
    return 0;
}
