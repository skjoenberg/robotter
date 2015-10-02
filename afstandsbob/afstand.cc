#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <time.h>
#include <stdio.h>

//using namespace irrklang;
using namespace cv;
using namespace std;

//#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

Mat imgOriginal;
Mat imgHSV;
Mat imgDst;
Mat imgThresholded;
Mat imgThresholded2;
Mat imgThresholded3;
Mat im_with_keypoints;
bool bSuccess;
std::vector<KeyPoint> keypoints;
SimpleBlobDetector::Params params;
int best;
int counter;
int avg[10];
Point2f blobpos;
int sliderman;
int witb;
int counter360;
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

// Player objects
// PlayerClient robert("172.16.187.128");
// Position2dProxy pp(&robert);
// IrProxy ir(&robert);
// BumperProxy bumper(&robert);

float cameraGO(VideoCapture* cap) {
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
	inRange(imgHSV, Scalar(0,255,0), Scalar(0,0,0), imgThresholded3);

	// Merge the thresholds
	imgThresholded = imgThresholded + imgThresholded2;

	// Morphological opening (removes small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	imshow("Filter", imgThresholded); //show the thresholded image

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	GaussianBlur(imgThresholded, imgThresholded, Size(iBlur, iBlur), 0, 0);

	/// Find contours
	findContours(imgThresholded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Find the convex hull object for each contour
	vector<vector<Point> >hull( contours.size() );
	for( int i = 0; i < contours.size(); i++ ) {
		convexHull( Mat(contours[i]), hull[i], false );
	}

	/// Draw contours + hull results
	best = 0;
	for (int i = 1; i < hull.size(); i++) {
		if (hull[i].size() > hull[best].size()) {
			best = i;
		}
	}
	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );


	int min = 480;
	int max = 0;
	int height; 
	if (hull.size()){ 
    	for(int i = 0; i < hull[best].size(); i++) {
			if(hull[best][i].y < min){
				min = hull[best][i].y;
			}
			if(hull[best][i].y > max){
				max = hull[best][i].y;
			}
    	}
    	height = max - min;
    	if (counter > 9) {
    		int sum = 0;
    		for(int i = 0; i < 10; i++) {
    			sum += avg[i];
    		}
    		printf("height = %d \n", sum / 10);
    		counter = 0;
    	}
    	if (height > 0) {
    		avg[counter] = height;
    		counter++;
    	}

	}
	drawContours(imgThresholded3, hull, best, color);


	imshow("Thres all", imgThresholded3); //show the thresholded image
	imshow("Original", imgOriginal); //show the original image

	if (keypoints.size() > 0) {
		blobpos = keypoints[best].pt;
	} else {
		return -1;
	}

	return blobpos.x;
}

void findBox() {
	// counter360++;
	// printf("%d\n", counter360);
	// pp.SetSpeed(0.0, 0.2);
}

void goLeft() {
	//    pp.SetSpeed(0.0, 0.2);
}

void goStraight() {
	/*    robert.Read();
		  if(!obsFront(&ir)) {
		  printf("Frem!");
		  forward(&pp);
		  } else {
		  pp.SetSpeed(0.0, 0.0);
		  while(true) {
		  printf("found the box master \n");
		  }
		  } */
}

void goRight() {
	//    pp.SetSpeed(0.0, -0.2);
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

	counter = 0;

	while (true) {
		iBlur = iBlur2;
		if (!(iBlur2 % 2))
			iBlur++;

		float blobX = cameraGO(&cap);
		// robot time!
		witb = -1;
		if (blobX < 220 && blobX > 0) {
			witb = 0;
		} else if (blobX > 220 && blobX < 420) {
			//blaze it up faggot
			witb = 1;
		} else if (blobX > 420) {
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
