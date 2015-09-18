#include "cv.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;

IplImage* GetThresholdedImage(IplImage* img)
{
     // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
    cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);
    cvReleaseImage(&imgHSV);
    return imgThreshed;
}

int main()
{
/*
      // Initialize capturing live feed from the camera
    CvCapture* capture = 0;

    capture = cvCaptureFromCAM(0);

    // Couldn't get a device? Throw an error and quit
    if(!capture)
    {
        printf("Could not initialize capturing...");
        return -1;
    }
    */

    VideoCapture cap(0); //capture the video from web cam


     // The two windows we'll be using
    cvNamedWindow("video");
    cvNamedWindow("thresh");

       while(true)
    {
        // Will hold a frame captured from the camera
        IplImage* frame = 0;
        frame = cvQueryFrame(capture);
        // If we couldn't grab a frame... quit
        if(!frame){
            break;
        }
        // Holds the yellow thresholded image (yellow = white, rest = black)
        IplImage* imgYellowThresh = GetThresholdedImage(frame);
}