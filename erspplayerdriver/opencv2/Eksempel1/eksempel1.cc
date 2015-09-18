#include <iostream>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace cv;

int main(int argc, char *argv[]) 
{
    // Get an OpenCV camera handle
    VideoCapture cam(-1);
    
    if (!cam.isOpened())
    {
        std::cout  << "Could not open camera" << std::endl;
        return -1;
    }

    // Initialise the GUI
    const char *WIN_RF = "Eksempel 1";
    namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(WIN_RF, 400       , 0);


    // For storing frames
    Mat frameReference;
    
    // The main loop
    while (cvWaitKey(4) == -1) {
        cam >> frameReference;
        
        if (frameReference.empty())
        {
            std::cout << " < < <  Game over!  > > > ";
                    break;
        }
        
        // Show frames
        imshow(WIN_RF, frameReference);
    }
    
    return 0;
}
