#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <libplayerc++/playerc++.h>
#include <time.h>
#include "scorpion.h"

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
    pp.SetSpeed(0.2, 0.0);
}

void goRight() {
    pp.SetSpeed(0.0, -0.2);
}

float cameraGo() {
    vector<float> xs;
    VideoCapture cap(0); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
        {
            cout << "Cannot open the web cam" << endl;
            return -1;
        }
    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

    int iLowH = 160;
    int iHighH = 179;

    int iLowS = 167;
    int iHighS = 255;

    int iLowV = 160;
    int iHighV = 255;

    int iLowH2 = 0;
    int iHighH2 = 10;

    int blur = 51;

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );

    for (int i = 0; i < 10; i++) {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video

        if (!bSuccess) {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat imgHSV;
        Mat imgDst;
        GaussianBlur(imgOriginal, imgDst, Size(blur, blur), 0, 0);

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;
        Mat imgThresholded2;
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
        inRange(imgHSV, Scalar(iLowH2, iLowS, iLowV), Scalar(iHighH2, iHighS, iHighV), imgThresholded2); //Threshold the image

        imgThresholded = imgThresholded + imgThresholded2;

        //morphological opening (removes small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        // Setup SimpleBlobDetector parameters.
        SimpleBlobDetector::Params params;

        // Change thresholds
        params.minThreshold = 20;
        params.maxThreshold = 160;

        // BlobColor
        params.blobColor = 255;

        // Filter by Area.
        params.filterByArea = true;
        params.minArea = 20;
        params.maxArea = 999999;

        params.filterByCircularity = false;
        params.filterByConvexity = false;
        params.filterByInertia = false;

        // SimpleBlobDetector detector(params); (til opencv2)
        Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

        // Storage for blobs
        std::vector<KeyPoint> keypoints;

        // Detect blobs
        detector->detect(imgThresholded, keypoints);

        int best = 0;
        for(int i = 0; i < keypoints.size(); i++) {
            if (keypoints[i].size > keypoints[best].size) {
                best = i;
            }
        }

        if (keypoints.size() > 0) {
            xs.push_back(keypoints[best].pt.x);
        }
    }
    if (xs.size() > 4) {
        float sum =0;
        for(int i = 0; i < xs.size(); i++) {
            sum += xs[i];
        }
        return sum / xs.size();
    } else {
        return -1.0;
    }
}

int main( int argc, char** argv ) {
    //    Streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    while (true) {
        float x = cameraGo();
        // robot time!

        int witb = -1;
        if (x < 220) {
            witb = 0;
        } else if (x > 220 && x < 420) {
            //blaze it up faggot
            witb = 1;
        } else if (x > 420) {
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
