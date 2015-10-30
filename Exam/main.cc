#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <iomanip>

//#include "index.h"
#include "random_numbers.h"
#include <libplayerc++/playerc++.h>
#include "scorpion.h"

#include "defines.h"
#include "main.h"
#include "robot.h"
#include "camera.h"
#include "particles.h"
#include "resampling.h"
#include "ui.h"

using namespace std;
using namespace PlayerCc;

/*
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */


const CvPoint landmarks [NUM_LANDMARKS] = {
    cvPoint (0, 300),
    cvPoint (0, 0),
    cvPoint (400, 300),
    cvPoint (400, 0)
};

/*************************\
 *      Main program     *
\*************************/
int main()
{
    // Debugging
    bool debug = true;

    // The GUI
    const char *map = "World map";
    const char *window = "Robot View";
    IplImage *world = cvCreateImage (cvSize (800, 500), IPL_DEPTH_8U, 3);
    cvNamedWindow (map, CV_WINDOW_AUTOSIZE);
    cvNamedWindow (window, CV_WINDOW_AUTOSIZE);
    cvMoveWindow (window, 20, 20);

    cout << "Initializing robot" << endl;
    //    Robot robert;

    // Initialize particles
    vector<particle> particles(NUM_PARTICLES);
    for (int i = 0; i < NUM_PARTICLES; i++) {
        // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
        particles[i].x = 2000.0*randf() - 1000;
        particles[i].y = 2000.0*randf() - 1000;
        particles[i].theta = 2.0*M_PI*randf() - M_PI;
        particles[i].weight = 1.0/(double)NUM_PARTICLES;
    }

    // Estimate position
    particle est_pose = estimate_pose (particles); // The estimate of the robots current pose

    // The camera interface
    camera cam;

    // Parameters
    const CvSize size = cvSize (320, 240);
    const double odometry_sigma = 1;

    // Draw map
    draw_world (est_pose, particles, world);

    // Modes
    bool search_mode = true, measure_mode = false;

    // Found landmarks
    bool found_red = false, found_green = false;


    int measure_counter;

    IplImage *im;

    // Main loop
    while (true) {
        // Get current position (x, y)
        //        robert.read();
        //        double x_before = robert.pp->GetXPos();
        //        double y_before = robert.pp->GetYPos();

        // LAV NOGET FLYTTELSE
        while (search_mode) {

            // Turning
            // if (!measure_mode) {
            //     // Get current angle
            //     //                robert.read();
            //     //                double theta_before = robert.pp->GetYaw();

            //     // Turn and get new angle
            //     //                robert.turnXradians(0.17);
            //     //                robert.read();

            //     // Move particles with the delta angle
            //     //                double deltatheta = robert.pp->GetYaw() - theta_before;
            //     for(int i = 0; i < particles.size(); i++) {
            //         //                    move_particle(particles[i], 0, 0, deltatheta);
            //     }

            //     // Add uncertainty
            //     add_uncertainty(particles, 0.1, 0.01);

            //     timespec hej = {3, 0};
            //     nanosleep(&hej, NULL);
            // }
            im = cam.get_colour();
            cam.empty();
            cvShowImage (window, im);
            //rgb_im = cam.get_colour ();

            // Detect landmarks
            double measured_distance, measured_angle;
            colour_prop cp;
            cout << "fra nu..." << endl;
            object::type ID = cam.get_object (im, cp, measured_distance, measured_angle);
            cout << "til nu!" << endl;
            // if (ID != object::none) {
            //     if (debug) {
            //         printf ("Measured distance: %f\n", measured_distance);
            //         printf ("Measured angle:    %f\n", measured_angle);
            //         printf ("Colour probabilities: %.3f %.3f %.3f\n", cp.red, cp.green, cp.blue);
            //     }

            //     // Horizontal / vertical
            //     if (debug) {
            //         if (ID == object::horizontal) {
            //             printf ("Landmark is horizontal\n");
            //         } else if (ID == object::vertical) {
            //             printf ("Landmark is vertical\n");
            //         } else  {
            //             printf ("Unknown landmark type!\n");
            //             continue;
            //         }
            //     }

            //     // Tror det her er underligt
            //     if (!measure_mode) {
            //         if ((cp.red > cp.green && !found_red) || (cp.green > cp.red && !found_green)) {
            //             measure_mode = true;
            //             measure_counter = 0;
            //         }
            //     } else {
            //         measure_counter++;
            //         if (measure_counter > 30) {
            //             if (cp.red > cp.green) {
            //                 found_red = true;
            //             } else {
            //                 found_green = true;
            //             }
            //             measure_mode = false;
            //         }
            //     }

            //     //
            //     float box_x, box_y;
            //     if (cp.red > cp.green) {
            //         box_x = 0.;
            //         box_y = 0.;
            //     } else {
            //         box_x = 300.;
            //         box_y = 0.;
            //     }

            //     // Resample particles
            //     resample(particles, box_x, box_y, measured_angle, measured_distance);
            //     add_uncertainty(particles, 10, 0.2);

            // } else { // end: if (found_landmark)
            //     // No observation - reset weights to uniform distribution
            //     for (int i = 0; i < NUM_PARTICLES; i++) {
            //         particles[i].weight = 1.0/(double)NUM_PARTICLES;
            //     }
            // } // end: if (not found_landmark)

            ////////////////
            // Draw stuff //
            ////////////////
            cout << "Updating images" << endl;
            cam.draw_object (im);

            // Estimate pose
            est_pose = estimate_pose (particles);

            // Visualisation
            draw_world (est_pose, particles, world);
            cvShowImage (map, world);
            int action = cvWaitKey (10);
            if (found_red && found_green) {
                search_mode = false;
            }
        } // end search mode inner loop (cirkel)

        if (debug) {
            cout << "Finished searching." << endl;
        }

        if (!search_mode) {
            if (debug) {
                cout << "Robotten kører!" << endl;
            }

            int target_x, target_y, deltax, deltay, dist, angletotarget, deltaangle;
            target_x = 150;
            target_y = 0;
            deltax = est_pose.x - target_x;
            deltay = est_pose.y - target_y;

            // Euclidean distance to box
            dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));

            // Angle between particle and box
            angletotarget = atan(deltay / deltax);

            // If deltax > 0, then the angle needs to be turned by a half circle.
            if (deltax > 0) {
                angletotarget -= M_PI;
            }

            // Difference in angle
            deltaangle = est_pose.theta - angletotarget;

            // The angles are between (-pi, pi)
            if (deltaangle > M_PI){
                deltaangle -= 2 * M_PI;
            } else if (deltaangle < -M_PI) {
                deltaangle += 2 * M_PI;
            }

            if (debug) {
                cout << "Turning: " << deltaangle << endl;
                cout << "Distance: " << dist << endl;
            }

            // Turning
            //            robert.turnXradians(deltaangle);

            // Moving forward
            //            robert.moveXcm(dist);

            search_mode = true;
            found_red = false;
            found_green = false;
        }

        // add_uncertainty(particles, 10, 0.2);
        // robert.read();
        // double deltax = robert.pp->GetXPos() - x_before;
        // double deltay = robert.pp->GetYPos() - y_before;
        // double deltatheta = robert.pp->GetYaw() - theta_before;

        // for(int i = 0; i < particles.size(); i++) {
        //     move_particle(particles[i], deltax, deltay, deltatheta);
        // }
    } // End: while (true)

 theend:
    // Stop the robot
    // XXX: Stop the robot

    // Clean up and return
    //cvReleaseImage (&rgb_im);
    //cvReleaseImage (&im);

    return 0;
}
