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
        // LAV NOGET FLYTTELSE
        robert.pp->SetSpeed(0.0, 0.1);
        while (search_mode) {
            // Turning
            // Get current angle
            robert.read();
            double theta_before = robert.pp->GetYaw();

            // Add uncertainty
            add_uncertainty(particles, 0.1, 0.01);

            im = cam.get_colour();
            cam.empty();
            cvShowImage (window, im);
            //rgb_im = cam.get_colour ();

            // Detect landmarks
            double measured_distance, measured_angle;
            colour_prop cp;

            object::type ID = cam.get_object (im, cp, measured_distance, measured_angle);

            if (ID != object::none) {
                if (debug) {
                    printf ("Measured distance: %f\n", measured_distance);
                    printf ("Measured angle:    %f\n", measured_angle);
                    printf ("Colour probabilities: %.3f %.3f %.3f\n", cp.red, cp.green, cp.blue);
                }

                // Horizontal / vertical
                if (debug) {
                    if (ID == object::horizontal) {
                        printf ("Landmark is horizontal\n");
                    } else if (ID == object::vertical) {
                        printf ("Landmark is vertical\n");
                    } else  {
                        printf ("Unknown landmark type!\n");
                        continue;
                    }
                }

                //
                float box_x, box_y;
                if (cp.red > cp.green) {
                    box_x = 0.;
                    box_y = 0.;
                } else {
                    box_x = 300.;
                    box_y = 0.;
                }

                // Resample particles
                resample(particles, box_x, box_y, measured_angle, measured_distance);
                add_uncertainty(particles, 10, 0.2);

            } else { // end: if (found_landmark)
                // No observation - reset weights to uniform distribution
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    particles[i].weight = 1.0/(double)NUM_PARTICLES;
                }
            } // end: if (not found_landmark)

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

            // Move particles with the delta angle
            robert.read();
            double deltatheta = robert.pp->GetYaw() - theta_before;
                for(int i = 0; i < particles.size(); i++) {
                    move_particle(particles[i], 0, 0, deltatheta);
                }

            // if (found_red && found_green) {
            //     search_mode = false;
            // }

        } // end search mode inner loop (cirkel)

        if (debug) {
            cout << "Finished searching." << endl;
        }
    } // End: while (true)

 theend:
    // Stop the robot
    // XXX: Stop the robot

    // Clean up and return
    //cvReleaseImage (&rgb_im);
    //cvReleaseImage (&im);

    return 0;
}
