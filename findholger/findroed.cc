#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <libplayerc++/playerc++.h>
#include <time.h>
#include "scorpion.h"
#include "movement.hpp"

using namespace PlayerCc;
using namespace cv;
using namespace std;

// Player objects
PlayerClient robert("172.16.187.128");
Position2dProxy pp(&robert);
IrProxy ir(&robert);
BumperProxy bumper(&robert);

void findBox() {
    pp.SetSpeed(0.0, 0.2);
}

void goLeft() {
    pp.SetSpeed(0.0, 0.2);
}

void goStraight() {
    if(!obsFront){ 
        pp.SetSpeed(0.2, 0.0);
    }
}

void goRight() {
    pp.SetSpeed(0.0, -0.2);
}

int main( int argc, char** argv ) {
    // Streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    // Capture the video from webcam
    VideoCapture cap(0);

    // Some camera errors
    if ( !cap.isOpened() )
        {
            cout << "Cannot open the web cam" << endl;
            return -1;
        }

    // Create a Control window
    namedWindow("Control", CV_WINDOW_AUTOSIZE);

    // Some Hue and Saturation settings
    int iLowH = 160;
    int iHighH = 179;

    int iLowS = 167;
    int iHighS = 255;

    int iLowV = 160;
    int iHighV = 255;

    int iLowH2 = 0;
    int iHighH2 = 10;

    // Blur variable (used in gaussian blur)
    int blur = 51;

    // Create trackbars in the Control window
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 179);

    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;

    //
    while (true) {
        Mat imgOriginal;
        Mat imgHSV;
        Mat imgDst;
        Mat imgThresholded;
        Mat imgThresholded2;
        Mat im_with_keypoints;
        bool bSuccess;
        std::vector<KeyPoint> keypoints;
        SimpleBlobDetector::Params params;
        int best;
        Point2f blobpos;
        int witb;

        // Read a new frame from video
        bSuccess = cap.read(imgOriginal);

        // Error handling for the camera
        if (!bSuccess) {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        // Filter the frame using guassian blur
        GaussianBlur(imgOriginal, imgDst, Size(blur, blur), 0, 0);

        // Convert the captured frame from BGR to HSV
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

        // Two thresholds are needed to succesfully capture the color red
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
        inRange(imgHSV, Scalar(iLowH2, iLowS, iLowV), Scalar(iHighH2, iHighS, iHighV), imgThresholded2);

        // Merge the thresholds
        imgThresholded = imgThresholded + imgThresholded2;

        // Morphological opening (removes small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        // Setup SimpleBlobDetector parameters.
        // Change thresholds
        params.minThreshold = 20;
        params.maxThreshold = 160;

        // BlobColor
        params.blobColor = 255;

        // Filter by Area (and other blob settings)
        params.filterByArea = true;
        params.minArea = 20;
        params.maxArea = 999999;

        params.filterByCircularity = false;
        params.filterByConvexity = false;
        params.filterByInertia = false;

        SimpleBlobDetector detector(params);
        //Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params); //Used with opencv3

        // Storage for blobs


        // Detect blobs
        detector.detect(imgThresholded, keypoints);

        // Draw detected blobs as red circles.
        // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
        // the size of the circle corresponds to the size of blob

        drawKeypoints(imgOriginal, keypoints, imgOriginal, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
        drawKeypoints(imgThresholded, keypoints, imgThresholded, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

        best = 0;
        for(int i = 0; i < keypoints.size(); i++) {
            if (keypoints[i].size > keypoints[best].size) {
                best = i;
            }
        }

        if (keypoints.size() > 0) {
            blobpos = keypoints[best].pt;
        }

        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original", imgOriginal); //show the original image

        // robot time!
        witb = -1;
        if (blobpos.x < 220) {
            witb = 0;
        } else if (blobpos.x > 220 && blobpos.x < 420) {
            //blaze it up faggot
            witb = 1;
        } else if (blobpos.x > 420) {
            witb = 2;
        }

        switch(witb) {
        case -1:
            findBox();
            break;
        case 0:
            goLeft();
            break;
        case 1:
            goStraight();
            break;
        case 2:
            goRight();
            break;
        default:
            cout << "u don goofed" << endl;
            return 0;
        }

        if (waitKey(30) == 27) {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    return 0;
}
