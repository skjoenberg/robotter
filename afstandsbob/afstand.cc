#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <time.h>


//using namespace irrklang;
using namespace cv;
using namespace std;

//#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

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
RNG rng(12345);

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

    // Merge the thresholds
    imgThresholded = imgThresholded + imgThresholded2;

    // Morphological opening (removes small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

   vector<vector<Point> > contours;
   vector<Vec4i> hierarchy;

   GaussianBlur(imgThresholded, imgThresholded, Size(iBlur, iBlur), 0, 0);

   /// Find contours
   findContours(imgThresholded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   /// Find the convex hull object for each contour
   vector<vector<Point> >hull( contours.size() );
   for( int i = 0; i < contours.size(); i++ )
       {  convexHull( Mat(contours[i]), hull[i], false ); }

   /// Draw contours + hull results
   best = 0;
   for (int i = 0; i < hull.size(); i++) {
       if (hull[i].size() > hull[best].size()) {
           best = i;
       }
   }

   Mat lolDst;

   drawContours(imgThresholded, hull, best, Scalar(180), 1, 8, vector<Vec4i>(), 0, Point() );

   vector<Vec2f> lines;

   HoughLines(imgOriginal, lines, 1, CV_PI/180, 100);

   for(int i = 0; i < lines.size(); i++) {
       float rho = lines[i][0], theta = lines[i][1];
       Point pt1, pt2;
       double a = cos(theta), b = sin(theta);
       double x0 = a*rho, y0 = b*rho;
       pt1.x = cvRound(x0 + 1000*(-b));
       pt1.y = cvRound(y0 + 1000*(a));
       pt2.x = cvRound(x0 - 1000*(-b));
       pt2.y = cvRound(y0 - 1000*(a));
       line(imgOriginal, pt1, pt2, Scalar(0,0,255), 10, CV_AA);
   }
   cout << "found " << lines.size() << " lines" << endl;

   imshow("Thresholded Image", imgThresholded); //show the thresholded image
   imshow("Original", imgOriginal); //show the original image

   return -1.0;
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
    createTrackbar("Blur", "Control", &sliderman, 500); //Hue (0 - 179)

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

        printf("witb: %d, blobpos.x: %f\n", witb, blobpos.x);
        switch(witb) {
        case -1:
            printf("FIND BOX!\n");
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
