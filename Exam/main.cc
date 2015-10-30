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


// Orientation: true is horisontal, false is vertical
CvPoint get_landmark(colour_prop cp, bool orientation) {
    CvPoint ret;
    if (orientation) {
        if (cp.red > cp.green) {
            ret.x = 400;
            ret.y = 0;
        } else {
            ret.x = 400;
            ret.y = 300;
        }
    } else {
        if (cp.red > cp.green) {
            ret.x = 0;
            ret.y = 300;
        } else {
            ret.x = 0;
            ret.y = 0;
        }
    }
    return ret;
}


const CvPoint landmarks [NUM_LANDMARKS] = {
    cvPoint (0, 300),
    cvPoint (0, 0),
    cvPoint (400, 300),
    cvPoint (400, 0)
};

int seen[NUM_LANDMARKS];

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
    Robot robert;

    // Initialize particles
    vector<particle> particles(NUM_PARTICLES);
    particles_init(particles);

    // Estimate position
    particle est_pose = estimate_pose (particles); // The estimate of the robots current pose

    // The camera interface
    camera cam;

    // Draw map
    draw_world (est_pose, particles, world);

    // Modes
    bool search_mode = true, driving_mode = false;

    int measure_counter;

    IplImage *im;

    double theta_before, delta_theta, theta_sum;

    robert.pp->ResetOdometry();

    int next = 0;

    // Main loop
    while (true) {
        theta_sum = 0;
        for (int i = 0; i < NUM_LANDMARKS; i++) {
            seen[i] = 0;
        }
        while (search_mode) {
            robert.pp->SetSpeed(0.0, 0.1);
            // Get current angle
            robert.read();
            theta_before = robert.pp->GetYaw();

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
                CvPoint box;
                // Horizontal / vertical
                if (ID == object::horizontal) {
                    box = get_landmark(cp, true);
                } else if (ID == object::vertical) {
                    box = get_landmark(cp, false);
                } else  {
                    printf ("Unknown landmark type!\n");
                    continue;
                }
                // Resample particles
                resample(particles, box.x, box.y, measured_angle, measured_distance);
                add_uncertainty(particles, 10, 0.2);
                for (int i = 0; i < NUM_LANDMARKS; i++) {
                    if (landmarks[i].x == box.x && landmarks[i].y == box.y) {
                        seen[i] = 1;
                    }
                }
            } else { // end: if (found_landmark)
                // No observation - reset weights to uniform distribution
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    particles[i].weight = 1.0/(double)NUM_PARTICLES;
                }
            } // end: if (not found_landmark)

            robert.read();
            delta_theta = robert.pp->GetYaw() - theta_before;

            if (delta_theta > M_PI){
                delta_theta -= 2 * M_PI;
            } else if (delta_theta < -M_PI) {
                delta_theta += 2 * M_PI;
            }
            move_particles(particles,0,0, -delta_theta);

            theta_sum += abs(delta_theta);

            ////////////////
            // Draw stuff //
            ////////////////
            cam.draw_object (im);

            // Estimate pose
            est_pose = estimate_pose (particles);

            // Visualisation
            draw_world (est_pose, particles, world);
            cvShowImage (map, world);
            int action = cvWaitKey (10);

            cout << theta_sum << endl;

            //exit searchmode if turned 360 degrees.
            if (abs(theta_sum) > (2 * M_PI + 0.1)) {
                cout << "arh man jeg så træt af at dreje" << endl;
                search_mode = false;
                driving_mode = true;
                robert.pp->SetSpeed(0.0, 0.0);
            }
        } // end search mode
        if (debug) {
            cout << "Finished searching." << endl;
        }
        // int sum_seen = 0;
        // for (int i = 0; i < NUM_LANDMARKS; i++) {
        //     sum_seen += seen[i];
        // }
        // if (sum_seen > 1) {
        //     driving_mode = true;
        // }
        while(driving_mode) {
            int target_x, target_y, deltax, deltay, dist;
            double angletotarget, deltaangle;

            target_x = landmarks[next].x;
            target_y = landmarks[next].y;
            deltax = est_pose.x - target_x;
            deltay = est_pose.y - target_y;

            cout << "robert estimerer at han står i x: " << est_pose.x << " y: " << est_pose.y << endl;
            cout << "target er x: " << target_x << " y: " << target_y << endl;

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

            if (dist < 60) {
                cout << "hurra vi har mødt landmark " << next << endl;
                next++;

            } else {
                double x_before, y_before, theta_before, moved_x, moved_y, driving_dist, turned_theta;
                robert.read();
                x_before = robert.pp->GetXPos();
                y_before = robert.pp->GetYPos();
                theta_before = robert.pp->GetYaw();
                cout << "deltaangle: " << deltaangle << endl;
                cout << "dist: " << driving_dist << endl;
                robert.turnXradians(deltaangle);
                robert.moveXcm(driving_dist);

                robert.read();
                moved_x = robert.pp->GetXPos() - x_before;
                moved_y = robert.pp->GetYPos() - y_before;
                turned_theta = robert.pp->GetYaw() - theta_before;
                move_particles(particles, moved_x, moved_y, turned_theta);
                add_uncertainty(particles, 10, 0.2);
            }
            driving_mode = false;
            search_mode = true;

        } // End Driving mode


    } // End: while (true)

 theend:
    // Stop the robot
    // XXX: Stop the robot

    // Clean up and return
    //cvReleaseImage (&rgb_im);
    //cvReleaseImage (&im);

    return 0;
    }
