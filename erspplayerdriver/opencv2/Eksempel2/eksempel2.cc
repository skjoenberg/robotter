#include <iostream>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include "opencv2/imgproc/imgproc.hpp"  // For the Canny edge detector

using namespace cv;


int edgeThresh = 1;
int lowThreshold=35;
int ratio = 3;
int kernel_size = 3;

int main(int argc, char *argv[]) 
{
    // Get an OpenCV camera handle
    VideoCapture cam(-1);
    
    if (!cam.isOpened())
    {
        std::cout  << "Could not open camera" << std::endl;
        return -1;
    }
    
    
    // Fetch the camera frame size
    Size refS = Size((int) cam.get(CV_CAP_PROP_FRAME_WIDTH),
        (int) cam.get(CV_CAP_PROP_FRAME_HEIGHT));

    std::cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height
              << std::endl;

    // Initialise the GUI
    const char *WIN_RF = "Eksempel 2";
    namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(WIN_RF, 400       , 0);


    // For storing frames
    Mat frameReference, gray_frame, edge_frame;
    
    // The main loop
    while (cvWaitKey(4) == -1) {
        cam >> frameReference;
        if (frameReference.empty())
        {
            std::cout << " < < <  Game over!  > > > ";
                    break;
        }
        
        /// Convert the image to grayscale
        cvtColor( frameReference, gray_frame, CV_BGR2GRAY );
        
        /// Reduce noise with a kernel 3x3
        blur( gray_frame, edge_frame, Size(3,3) );
        
        /// Canny detector
        Canny( edge_frame, edge_frame, lowThreshold, lowThreshold*ratio, kernel_size );
        
        // Show edge frames
        imshow(WIN_RF, edge_frame);
    }
    
    return 0;
}
