// This works with OpenCV 2 and above
//#include <opencv2/core/core.hpp>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <string>
#include <vector>
#include <iostream>

using namespace cv;


int main() {
    std::string img_path = "/home/sebastian/Pictures/Webcam/";
    //std::string img_path = "../data/";
    std::vector<std::string> img_list;

    // Mac Book Pro camera (full resolution)
    /*
    img_list.push_back("testimg1.jpg");
    img_list.push_back("testimg2.jpg");
    img_list.push_back("testimg3.jpg");
    img_list.push_back("testimg4.jpg");
    img_list.push_back("testimg5.jpg");
    img_list.push_back("testimg6.jpg");
    img_list.push_back("testimg7.jpg");
    img_list.push_back("testimg8.jpg");
    */

    // Frindo Raspberry Pi camera
    img_list.push_back("1.jpg");
    img_list.push_back("2.jpg");
    img_list.push_back("3.jpg");
    img_list.push_back("4.jpg");
    img_list.push_back("5.jpg");
    img_list.push_back("6.jpg");
    img_list.push_back("7.jpg");
//    img_list.push_back("8.jpg");
    img_list.push_back("9.jpg");
    img_list.push_back("10.jpg");



    std::string winname = "Calibration image";
    namedWindow(winname);

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);;
    Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
    std::vector<Mat > rvecs, tvecs;

    Mat img, gray;

    std::vector<Point2f> corners;
    std::vector<std::vector<Point2f> > corners_list;

    Size patternSize(4, 3); // Number of internal corner points in the calibration checkerboard pattern
    int patternUnit = 50; // measured in mm


    for (int i = 0; i < img_list.size(); i++) {
        img = imread(img_path + img_list[i]);
        cvtColor(img, gray, CV_BGR2GRAY); // Convert to gray scale


        bool patternFound = findChessboardCorners(img, patternSize, corners);
        if (patternFound)
            cornerSubPix(gray, corners, Size(5,5), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));


        drawChessboardCorners(img, patternSize, corners, patternFound);

        imshow(winname, img);


        corners_list.push_back(corners);


        cvWaitKey(); // Wait forever for a key press
    }


    // Create a list of checkerboard corner points in object 3D coordinates
    std::vector<Point3f> patternPoints;

    for (int i = 0; i < patternSize.height; i++) {
        for (int j = 0; j < patternSize.width; j++) {
            Point3f pPoint(j * patternUnit, i * patternUnit, 0.0f);
            patternPoints.push_back(pPoint);
        }
    }

    std::vector<std::vector<Point3f> > patterns_list;

    // Populate the patterns list by duplicating as many times as we have images
    for (int i = 0; i < corners_list.size(); i++)
        patterns_list.push_back(patternPoints);



    // Calibrate camera instrinsic parameters based on analysed images
    Size imgSize(img.cols, img.rows);
    double rms = calibrateCamera(patterns_list, corners_list, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "Calibration reprojection error = " << rms << std::endl;


    // Save results in YAML format
    FileStorage fs( "calibration_params.yml", FileStorage::WRITE );

    fs << "ImageSize" << imgSize;
    fs << "reprojection_error" << rms;
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;



    return 0;
}
