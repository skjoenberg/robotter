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

int drive_dist = 200;

/*
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */

// Draw particles on map
void draw_particles(camera &cam, IplImage *im, IplImage *world, const char *map, vector<particle> &particles, particle &est_pose) {
            cam.draw_object (im);

            // Visualisation
            draw_world (est_pose, particles, world);
            cvShowImage (map, world);
            int action = cvWaitKey (10);
}

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

// Landmarks
const CvPoint landmarks [NUM_LANDMARKS] = {
    cvPoint (0, 300),
    cvPoint (0, 0),
    cvPoint (400, 300),
    cvPoint (400, 0)
};

// const CvPoint landmarks [NUM_LANDMARKS] = {
//     cvPoint (400, 0),
//     cvPoint (400, 300),
//     cvPoint (0, 0),
//     cvPoint (0, 300),
// };

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

    // Image stuff
    IplImage *im;

    // Modes
    bool search_mode = true, driving_mode = false , obstacle_mode = false, test_mode = false;

    double theta_before, delta_theta, theta_sum;
    robert.pp->ResetOdometry();

    // Used for landmark routes
    int next = 0;

    cout << landmarks[0].x << " " << landmarks[0].y << endl;
    cout << landmarks[1].x << " " << landmarks[1].y << endl;
    cout << landmarks[2].x << " " << landmarks[2].y << endl;
    cout << landmarks[3].x << " " << landmarks[3].y << endl;

    // Main loop
    while (true) {
        theta_sum = 0;
        for (int i = 0; i < NUM_LANDMARKS; i++) {
            seen[i] = 0;
        }

        while (search_mode) {
            drive_dist = 200;
            robert.pp->SetSpeed(0.0, 0.12);
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

            // Farve om natten
            cp.red -= 0.1;

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

            move_particles(particles,0,0, -delta_theta * THETA_MULTIPLIER);
            theta_sum += abs(delta_theta);

            // Estimate position
            est_pose = estimate_pose (particles);

            // Draw particles
            draw_particles(cam, im, world, map, particles, est_pose);

            //            cout << theta_sum << endl;

            //exit searchmode if turned 360 degrees.
            if (abs(theta_sum) > (2 * M_PI + 0.1)) {
                cout << "arh man jeg så træt af at dreje" << endl;
                search_mode = false;
                obstacle_mode = false;
                driving_mode = true;
                test_mode = false;
                robert.pp->SetSpeed(0.0, 0.0);
            }
            if (test_mode) {
                robert.pp->SetSpeed(0.0, 0.0);
                search_mode = false;
            }
        } // end search mode

        while(driving_mode) {
            cout << "Driving mode engaged" << endl;
            int target_x, target_y, deltax, deltay, dist;
            double angletotarget, deltaangle;

            target_x = landmarks[next].x;
            target_y = landmarks[next].y;
            deltax = est_pose.x - target_x;
            deltay = est_pose.y - target_y;

            // Euclidean distance to box
            dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));

            // Check if the robot is close enough to the landmark
            if (dist < 100) {
                cout << "Jeg har nu mødt landmark nr. " << (next+1) << endl;

                // Update next landmark
                if (next < NUM_LANDMARKS) {
                    next++;
                } else {
                    cout << "Mester, min opgave her er færdig. Jeg må nu forlade dig. Jeg har kone og børn i cyberspace.\n Farvel Mester, jeg vil savne dig. :'(" << endl;
                    //                    goto theend;
                }

                cout << "Jeg kører nu efter landmark nr. " << (next+1) << endl;

                // Change target to next landmark
                target_x = landmarks[next].x;
                target_y = landmarks[next].y;
                deltax = est_pose.x - target_x;
                deltay = est_pose.y - target_y;
                dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));
            }

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

            double x_before, y_before, theta_before, moved_x, moved_y, driving_dist, turned_theta;
            int obstacle;

            robert.read();
            x_before = robert.pp->GetXPos();
            y_before = robert.pp->GetYPos();
            theta_before = robert.pp->GetYaw();

            cout << "deltaangle: " << deltaangle << endl;
            cout << "dist: " << dist << endl;

            int magic = -100;
            driving_dist = std::min((dist+magic), drive_dist);
            cout << "driving_dist: " << driving_dist << endl;

            // Turn and drive
            robert.turnXradians(deltaangle);
            obstacle = robert.moveXcm(driving_dist, 0.2);

            // Update particles by odometry
            robert.read();
            moved_x = robert.pp->GetXPos() - x_before;
            moved_y = robert.pp->GetYPos() - y_before;
            turned_theta = robert.pp->GetYaw() - theta_before;
            move_particles(particles, abs(moved_x), abs(moved_y), -turned_theta * THETA_MULTIPLIER);
            add_uncertainty(particles, 10, 0.2);

            // Estimate position
            est_pose = estimate_pose (particles);
            deltax = est_pose.x - target_x;
            deltay = est_pose.y - target_y;

            // Euclidean distance to box
            dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));

            // Draw particles
            draw_particles(cam, im, world, map, particles, est_pose);

            if(obstacle == -1 && dist > 80 && drive_dist > 100) {
                // Obstacle found. Stay in obstacle mode
                obstacle_mode = true;
                driving_mode = false;
                search_mode = false;
                drive_dist -= 20;
            } else{
                // Switch to search mode
                cout << "Search mode engaged" << endl;
                obstacle_mode = false;
                driving_mode = false;
                search_mode = true;
            }
        } // End Driving mode

        while(obstacle_mode){
            cout << "Obstacle mode engaged" << endl;
            cout << "Our landmark is next: " << (next+1) << endl;
            // Variables
            double x_before, y_before, theta_before, moved_x, moved_y, driving_dist, turned_theta;
            // Get position from odometry
            robert.read();
            x_before = robert.pp->GetXPos();
            y_before = robert.pp->GetYPos();
            theta_before = robert.pp->GetYaw();

            int obstacle;
            // Move away from obstacle
            robert.turnObstacle();
            obstacle = robert.moveXcm(55, 0.1);

            // Get new position from odometry and update particles
            robert.read();
            moved_x = robert.pp->GetXPos() - x_before;
            moved_y = robert.pp->GetYPos() - y_before;
            turned_theta = robert.pp->GetYaw() - theta_before;
            move_particles(particles, abs(moved_x), abs(moved_y), -turned_theta * THETA_MULTIPLIER);
            add_uncertainty(particles, 10, 0.2);

            double target_x, target_y, deltax, deltay, dist;
            target_x = landmarks[next].x;
            target_y = landmarks[next].y;

            // Estimate position
            est_pose = estimate_pose (particles);
            deltax = est_pose.x - target_x;
            deltay = est_pose.y - target_y;

            // Euclidean distance to box
            dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));
            // Draw particles
            draw_particles(cam, im, world, map, particles, est_pose);

            // Switch to driving mode

            if (obstacle == 1 || dist < 80 || drive_dist < 100) {
                // Obstacle found. Stay in obstacle mode
                cout << "Driving mode engaged" << endl;
                obstacle_mode = false;
                driving_mode = true;
                search_mode = false;
                drive_dist -= 20;
            }
        } // End obstacle mode

        while (test_mode) {
            cout << "test mode engaged" << endl;
            // Variables
            double x_before, y_before, theta_before, moved_x, moved_y, driving_dist, turned_theta;

            // Get position from odometry
            robert.read();
            x_before = robert.pp->GetXPos();
            y_before = robert.pp->GetYPos();
            theta_before = robert.pp->GetYaw();
            cout << "x before: " << x_before << endl;
            cout << "y before: " << y_before << endl;

            robert.moveXcm(100, 0.2);

            // Get new position from odometry and update particles
            robert.read();
            moved_x = robert.pp->GetXPos() - x_before;
            moved_y = robert.pp->GetYPos() - y_before;
            turned_theta = robert.pp->GetYaw() - theta_before;
            cout << "x after: " << robert.pp->GetXPos() << endl;
            cout << "y after: " << robert.pp->GetYPos() << endl;
            cout << "x moved: " << moved_x << endl;
            cout << "y moved: " << moved_y << endl;

            est_pose = estimate_pose (particles);
            cout << "estimated position before: (" << est_pose.x << ", " << est_pose.y << ")" << endl;
            move_particles(particles, abs(moved_x), abs(moved_y), -turned_theta * THETA_MULTIPLIER);
            est_pose = estimate_pose (particles);
            cout << "estimated position after: (" << est_pose.x << ", " << est_pose.y << ")" << endl;
            // Estimate position
            est_pose = estimate_pose (particles);
            // Draw particles
            draw_particles(cam, im, world, map, particles, est_pose);

            // Switch to driving mode

            search_mode = true;
            test_mode = false;

        } // end test mode
    } // End: while (true)

 theend:
    // Stop the robot
    // XXX: Stop the robot

    // Clean up and return
    //cvReleaseImage (&rgb_im);
    //cvReleaseImage (&im);

    return 0;
}
