#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <time.h>
#include <stdio.h>
#include <algorithm>
#include <utility>
#include "movement.h"
#include <libplayerc++/playerc++.h>

//using namespace irrklang;
using namespace cv;
using namespace std;
using namespace PlayerCc;

//#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Paper information
const double fl = 704.4;
const double paperheight = 21.0;

//
#define FRAMES 7
#define MINIMUM_AREA 3000
#define MAXIMUM_AREA 100000

// Random variables
int best;
int avg[10];
Point2f blobpos;
int sliderman;
int witb;
int counter360;
bool bSuccess;

// Images
Mat imgOriginal;
Mat imgHSV;
Mat imgDst;
Mat imgThresholded;
Mat imgThresholded2;
Mat imgThresholded3;

// Filter settings
int iLowH;
int iHighH;
int iLowS;
int iHighS;
int iLowV;
int iHighV;
int iLowH2;
int iHighH2;
int iBlur;
int iBlur2 = 20;
RNG rng(12345);

int bestRight;
int bestLeft;
int bestHeight;
int bestMid;
vector<int> midCalc;
int currentDist;

// Time
timespec turn_sleep = { 4, 0 };
timespec forward_sleep = { 2, 0 };
timespec stop_sleep = { 3, 0 };

// Player objects
PlayerClient robert("172.16.187.128");
Position2dProxy pp(&robert);
IrProxy ir(&robert);
BumperProxy bumper(&robert);

double distance(double height) {
    return (fl * paperheight) / height;
}


vector<vector<Point> > convexHulls() {
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;


    // Find contours
    findContours(imgThresholded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<vector<Point> > hull(contours.size());
    vector<vector<Point> > cont2(contours.size());
    int areas[contours.size()];

    // Find the convex hull object for each contour
    for(int i = 0; i < contours.size(); i++) {
        convexHull(Mat(contours[i]), hull[i], false);
    }

    // Find the biggest hull
    best = 0;
    for (int i = 0; i < hull.size(); i++) {
        approxPolyDP(Mat(hull[i]), cont2[i], 0.001, true);
        areas[i] = fabs(contourArea(Mat(cont2[i])));
        if(areas[i] > areas[best]) {
            best = i;
        }
    }
    if (areas[best] < MINIMUM_AREA || areas[best] > MAXIMUM_AREA) {
        best = -1;
    }
    return hull;
}


pair<int, int> hullHeights(vector<Point>* bestHull) {
    int middle, width, offset;
    int lowright = 480;
    int highright = 0;
    int lowleft = 480;
    int highleft = 0;
    int leftX = 640;
    int rightX = 0;

    Point point;
    pair<int, int> result;

    for (int i = 0; i < bestHull->size(); i++) {
        point = bestHull->at(i);
        if (rightX < point.x)
            rightX = point.x;
        if (leftX > point.x)
            leftX = point.x;
    }

    middle = (rightX + leftX) / 2;
    midCalc.push_back(middle);
    width = rightX - leftX;
    offset = width / 4;

    for (int i = 0; i < bestHull->size(); i++) {
        point = bestHull->at(i);
        if (point.x < middle - offset) {
            if (point.y < lowright) {
                lowright = point.y;
            }
            if (point.y > highright) {
                highright = point.y;
            }

        } else if (point.x > middle + offset) {
            if (point.y < lowleft) {
                lowleft = point.y;
            }
            if (point.y > highleft) {
                highleft = point.y;
            }
        }
    }

    result.first = highright - lowright;
    result.second = highleft - lowleft;

    return result;
}

// Captures frames from the webcam
void cameraGO(VideoCapture* cap) {
    vector<vector<Point> > hull;
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
    pair<int, int> currentHull;
    bestRight = 0;
    bestLeft = 0;
    bestHeight = 0;
    currentDist;

    // Capture the amount of frames specified in "frames"
    for (int k = 0; k < FRAMES; k++) {
        // Read a new frame from video
        bSuccess = cap->read(imgOriginal);

        // Error handling for the camera
        if (!bSuccess) {
            cout << "Cannot read a frame from video stream" << endl;
        }

        // Filter the frame using guassian blur
        GaussianBlur(imgOriginal, imgDst, Size(iBlur, iBlur), 0, 0);

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

        // Create a black/blank image
        inRange(imgHSV, Scalar(0,255,0), Scalar(0,0,0), imgThresholded3);

        GaussianBlur(imgThresholded, imgThresholded, Size(iBlur, iBlur), 0, 0);

        hull = convexHulls();

        if (hull.size() && best >= 0){
            currentHull = hullHeights(&hull[best]);
            if (currentHull.first > bestRight) {
                bestRight = currentHull.first;
            }
            if (currentHull.second > bestLeft) {
                bestLeft = currentHull.second;
            }
        }

        drawContours(imgThresholded3, hull, best, color);

        // Show the images
        //show the thresholded image
        imshow("Filter", imgThresholded);
        //show the thresholded image
        imshow("Thres all", imgThresholded3);
        //show the original image
        imshow("Original", imgOriginal);
    }

    if (bestRight > bestLeft) {
        bestHeight = bestRight;
    } else {
        bestHeight = bestLeft;
    }

    if(midCalc.size()){
    	sort(midCalc.begin(), midCalc.end());
    	bestMid = midCalc[4];
    	midCalc.clear();
	}
	if(distance(bestHeight)){
		currentDist = distance(bestHeight);
	}
    printf("Left height is: %d\nRight height is: %d\n", bestLeft, bestRight);
    printf("we are %f cm from the paper \n", distance(bestHeight));
    return;
}

void findBox() {
	counter360++;
	printf("%d\n", counter360);
	pp.SetSpeed(0.0, 0.2);
}

void goLeft() {
	pp.SetSpeed(0.0, 0.2);
}

void goStraight() {
	robert.Read();
	if(currentDist > 150) {
		printf("Frem!");
		forward(&pp);
        nanosleep(&forward_sleep, NULL);
        pp.SetSpeed(0.0, 0.0);
	} else {
        if (abs(bestRight-bestLeft) > 5) {
            if((bestRight < bestLeft)) {
                //left er størst
                pp.SetSpeed(0,0,-0.2);
                nanosleep(&turn_sleep, NULL);
                pp.SetSpeed(0.2,0.0);
                nanosleep(&forward_sleep, NULL);
                pp.SetSpeed(0.0,0.2);
                nanosleep(&forward_sleep, NULL);
                pp.SetSpeed(0.0,0.0);
            } else {
                //right er størst
                pp.SetSpeed(0,0,0.2);
                nanosleep(&forward_sleep, NULL);
                pp.SetSpeed(0.2,0.0);
                nanosleep(&forward_sleep, NULL);
                pp.SetSpeed(0.0,-0.2);
                nanosleep(&forward_sleep, NULL);
                pp.SetSpeed(0.0,0.0);
            }
        } else {
            pp.SetSpeed(0.0,0.0);
            nanosleep(&stop_sleep, NULL);
        }

	}
}

void goRight() {
	pp.SetSpeed(0.0, -0.2);
}

int main( int argc, char** argv ) {
	// Streaming data instead of buffering
	// robert.SetDataMode(PLAYER_DATAMODE_PULL);
	// robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

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
	iLowH = 25;
	iHighH = 75;

	iLowS = 50;
	iHighS = 255;

	iLowV = 90;
	iHighV = 255;

	iLowH2 = 0;
	iHighH2 = 0;
	// Blur variable (used in gaussian blur)
	iBlur = 51;

	sliderman = 300;

	// Create trackbars in the Control window
	createTrackbar("SE HER", "Control", &iBlur2, 100); //Hue (0 - 179)

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

	counter360 = 0;

	/*
	  ISoundEngine* engine = createIrrKlangDevice();
	  if (!engine) {
	  printf("Could not startup engine\n");
	  return 0; // error starting up the engine
	  }
	*/

	printf("Starting!\n");

	while (true) {
		iBlur = iBlur2;
		if (!(iBlur2 % 2))
			iBlur++;

        cameraGO(&cap);
		// robot time!
		 witb = -1;
		if (bestMid < 220 && bestMid > 0) {
			witb = 0;
		} else if (bestMid > 220 && bestMid < 420) {
			//blaze it up faggot
			witb = 1;
		} else if (bestMid > 420) {
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
