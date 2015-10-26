#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <iomanip>

//#include "index.h"
#include "robot.h"
#include "camera.h"
#include "particles.h"
#include "random_numbers.h"

#include <libplayerc++/playerc++.h>
#include "scorpion.h"

/*
 * Some colors
 */
#define CRED     CV_RGB(255, 0, 0)
#define CGREEN   CV_RGB(0, 255, 0)
#define CBLUE    CV_RGB(0, 0, 255)
#define CCYAN    CV_RGB(0, 255, 255)
#define CYELLOW  CV_RGB(255, 255, 0)
#define CMAGENTA CV_RGB(255, 0, 255)
#define CWHITE   CV_RGB(255, 255, 255)
#define CBLACK   CV_RGB(0, 0, 0)

/*
 * Keyboard constants
 */
#define KEY_UP    65362
#define KEY_DOWN  65364
#define KEY_LEFT  65361
#define KEY_RIGHT 65363

#define SIGMA 20.0
#define SIGMA_THETA 0.3

#define TARGET_X 150
#define TARGET_Y 0

Robot robert;

using namespace std;
using namespace PlayerCc;

/*
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */
#define num_landmarks 2
const CvPoint landmarks [num_landmarks] = {
    cvPoint (0, 0),
    cvPoint (300, 0),
};

/*
 * Colour map for drawing particles. This function determines the colour of a
 * particle from its weight.
 */
CvScalar jet (const double x)
{
    const double r = (x >= 3.0/8.0 && x < 5.0/8.0) * (4.0 * x - 3.0/2.0)
        + (x >= 5.0/8.0 && x < 7.0/8.0)
        + (x >= 7.0/8.0) * (-4.0 * x + 9.0/2.0);
    const double g = (x >= 1.0/8.0 && x < 3.0/8.0) * (4.0 * x - 1.0/2.0)
        + (x >= 3.0/8.0 && x < 5.0/8.0)
        + (x >= 5.0/8.0 && x < 7.0/8.0) * (-4.0 * x + 7.0/2.0);
    const double b = (x < 1.0/8.0) * (4.0 * x + 1.0/2.0)
        + (x >= 1.0/8.0 && x < 3.0/8.0)
        + (x >= 3.0/8.0 && x < 5.0/8.0) * (-4.0 * x + 5.0/2.0);

    return CV_RGB (255.0*r, 255.0*g, 255.0*b);
}

/*
 * Visualization.
 * This functions draws robots position in the world.
 */
void draw_world (particle &est_pose, std::vector<particle> &particles, IplImage *im)
{
    const int offset = 100;

    // White background
    cvSet (im, CWHITE);

    // Find largest weight
    const int len = particles.size ();
    double max_weight = particles [0].weight;
    for (int i = 1; i < len; i++)
        max_weight = std::max (max_weight, particles [i].weight);

    // Draw particles
    for (int i = 0; i < len; i++)
        {
            const int x = (int)particles[i].x + offset;
            const int y = (int)particles[i].y + offset;
            const CvScalar colour = jet (particles[i].weight / max_weight);
            cvCircle (im, cvPoint (x,y), 2, colour, 2);
            const CvPoint b = cvPoint ((int)(particles[i].x + 15.0*cos(particles[i].theta))+offset,
                                       (int)(particles[i].y + 15.0*sin(particles[i].theta))+offset);
            cvLine   (im, cvPoint (x,y), b, colour, 2);
        }

    // Draw landmarks
    const CvPoint lm0 = cvPoint (landmarks[0].x+offset, landmarks[0].y+offset);
    const CvPoint lm1 = cvPoint (landmarks[1].x+offset, landmarks[1].y+offset);
    cvCircle (im, lm0, 5, CRED, 2);
    cvCircle (im, lm1, 5, CGREEN, 2);

    // Draw estimated robot pose
    const CvPoint a = cvPoint ((int)est_pose.x+offset, (int)est_pose.y+offset);
    const CvPoint b = cvPoint ((int)(est_pose.x + 15.0*cos(est_pose.theta))+offset,
                               (int)(est_pose.y + 15.0*sin(est_pose.theta))+offset);
    cvCircle (im, a, 5, CMAGENTA, 2);
    cvLine   (im, a, b, CMAGENTA, 2);
}

// void move(particle est_pose) {
//     double dx = (double)(est_pose.x - TARGET_X);
//     double dy = (double)(est_pose.x - TARGET_Y);
//     double targetangle = atan((dy)/(dx));
//     if (est_pose.x > TARGET_X) {
//         targetangle -= M_PI;
//     }
//     if (abs(est_pose.theta - targetangle) > M_PI / 6) {
//         turnXRadians(est_pose.theta - targetangle);
//         return;
//     }
//     robert.moveXcm(afstand til target)
//     return;
// }

void search() {
    int boxes = 0;
    while (boxes < 2) {
        // Snak med kamera
        // If only 1 box is spotted try to move into a new pos.
        if (boxes == 1) {
            if(measured_distance > 100){
                robert.moveXcm(measured_distance - 100);  
            }
            turnXRadians(0.25 * M_PI);
            robert.moveXcm(50);
            return;           
        }
        turnXRadians(0.17 * M_PI);
        // TODO: Lav noget check om der er drejet 360 uden at finde noget
    }
    //Hvis 2 bokse så move()
}

/*************************\
 *      Main program     *
\*************************/
int main()
{
    // The GUI
    const char *map = "World map";
    const char *window = "Robot View";
    IplImage *world = cvCreateImage (cvSize (500,500), IPL_DEPTH_8U, 3);
    cvNamedWindow (map, CV_WINDOW_AUTOSIZE);
    cvNamedWindow (window, CV_WINDOW_AUTOSIZE);
    cvMoveWindow (window, 20, 20);

    // Initialize particles
    const int num_particles = 2000;
    std::vector<particle> particles(num_particles);
    for (int i = 0; i < num_particles; i++)
        {
            // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
            particles[i].x = 2000.0*randf() - 1000;
            particles[i].y = 2000.0*randf() - 1000;
            particles[i].theta = 2.0*M_PI*randf() - M_PI;
            particles[i].weight = 1.0/(double)num_particles;
        }
    particle est_pose = estimate_pose (particles); // The estimate of the robots current pose

    // The camera interface
    camera cam;

    // Parameters
    const CvSize size = cvSize (320, 240);
    const double odometry_sigma = 1;

    // Allocate stuff
    //IplImage *rgb_im = cvCreateImage (size, IPL_DEPTH_8U, 3);

    // Initialize player (XXX: You do this)

    // Driving parameters
    double velocity = 15; // cm/sec
    const double acceleration = 12; // cm/sec^2
    double angular_velocity = 0.0; // radians/sec
    const double angular_acceleration = M_PI/2.0; // radians/sec^2

    // Draw map
    draw_world (est_pose, particles, world);

    // Main loop
    while (true)
        {
            robert.read();
            double x_before = robert.pp->GetXPos();
            double y_before = robert.pp->GetYPos();
            double theta_before = robert.pp->GetYaw();

            // LAV NOGET FLYTTELSE
            // HVAD LAVER DET HER???

            robert.read();
            double deltax = robert.pp->GetXPos() - x_before;
            double deltay = robert.pp->GetYPos() - y_before;
            double deltatheta = robert.pp->GetYaw() - theta_before;

            for(int i = 0; i < particles.size(); i++) {
                 move_particle(particles[i], deltax, deltay, deltatheta);
            }
            add_uncertainty(particles, 10, 0.2);

            // Grab image
            IplImage *im = cam.get_colour ();
            //rgb_im = cam.get_colour ();

            // Do landmark detection
            double measured_distance, measured_angle;
            colour_prop cp;
            object::type ID = cam.get_object (im, cp, measured_distance, measured_angle);
            if (ID != object::none)
                {
                    printf ("Measured distance: %f\n", measured_distance);
                    printf ("Measured angle:    %f\n", measured_angle);
                    printf ("Colour probabilities: %.3f %.3f %.3f\n", cp.red, cp.green, cp.blue);

                    if (ID == object::horizontal) {
                            printf ("Landmark is horizontal\n");
                    } else if (ID == object::vertical) {
                            printf ("Landmark is vertical\n");
                    } else  {
                            printf ("Unknown landmark type!\n");
                            continue;
                    }

                    float box_x, box_y;
                    if (cp.red > cp.green) {
                        box_x = 0.;
                        box_y = 0.;
                    } else {
                        box_x = 300.;
                        box_y = 0.;
                    }

                    // Compute particle weights
                    // XXX: You do this
                    /* Vægten er givet ved den funktion der står opgaven */
                    double tmpweight;
                    double sum = 0;
                    double dist;
                    double gaussman = 1. / sqrt(2. * M_PI * pow(SIGMA, 2.));
                    double angletobox;
                    double deltaangle;
                    double distweight;
                    double angleweight;
                    double deltax, deltay;

                    for (int i = 0; i < particles.size(); i++) {
                        // Measure euclidean distance to landmark
                        deltax = particles[i].x - box_x;
                        deltay = particles[i].y - box_y;

                        // Euclidean distance to box
                        dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));

                        // Angle between particle and box
                        angletobox = atan(deltay / deltax);

                        // If deltax > 0, then the angle needs to be turned by a half circle.
                        if (deltax > 0) {
                            angletobox -= M_PI;
                        }

                        // Difference in angle
                        deltaangle = particles[i].theta - angletobox;

                        // The angles are between (-pi, pi)
                        if (deltaangle > M_PI){
                            deltaangle -= 2 * M_PI;
                        } else if (deltaangle < -M_PI) {
                            deltaangle += 2 * M_PI;
                        }

                        // Debugging
                        if (deltaangle > M_PI) {
                            cout << deltaangle << endl;
                        }
                        if (deltaangle < -M_PI) {
                            cout << deltaangle << endl;
                        }

                        // Calculate weight of the current particle
                        angleweight = gaussman * exp(-((pow(measured_angle - deltaangle, 2.0) / (2.0 * pow(SIGMA_THETA, 2.0)))));
                        distweight = gaussman * exp(-((pow(measured_distance - dist, 2.0) / (2.0 * pow(SIGMA, 2.0)))));
                        //cout << "angleweight: " <<  angleweight << endl;
                        //cout << "deltaangle: " <<  deltaangle << endl;
                        //cout << "angletobox: " <<  angletobox << endl;
                        //cout << "particles theta: " <<  particles[i].theta << endl;
                        //cout << "particles x: " <<  particles[i].x << endl;
                        //cout << "particles y: " <<  particles[i].y << endl << endl;
                        tmpweight = angleweight*distweight;

                        // Add the weight to a sum (used later on to normalize weights)
                        sum += tmpweight;

                        // Save the weight in a particle array
                        particles[i].weight = tmpweight;

                        // ## Debugging! ##
                        // std::cout << "en weight er = " << tmpweight << std::endl;
                        // std::cout << "en distance er = " << dist << std::endl;
                        // std::cout << "measured dist er = " << measured_distance << std::endl;
                    }

                    for (int i = 0; i < particles.size(); i++) {
                        particles[i].weight = particles[i].weight / sum;
                    }

                    cout << "VI HAR " << particles.size() << " PARTIKLER MOTHERFUCCKER" << endl;

                    // Resampling
                    // XXX: You do this
                    /* Lav cumsum og rand [0, 1] for at se hvilken partikel du skal duplikere */
                    std::vector<particle> resamples;
                    vector<pair<double, int> > cumsum;
                    double cum = 0;
                    int count = 0;
                    for(int i = 0; i < particles.size(); i++) {
                        if (particles[i].weight > exp(-20)) {
                            cum += particles[i].weight;
                            cumsum.push_back(pair<double, int>(cum, i));
                            count++;
                        }
                    }

                    cout << "counted " << count << " particles" << endl;
                    std::cout << "cum: " << cum << std::endl;

                    resamples.clear();
                    for (int i = 0; i < num_particles; i++) {
                        float r = randf();
                        int j = 0;

                        while(cumsum[j].first < r && j < cumsum.size()) {
                            j++;
                        }

                        int tmpx = particles[cumsum[j].second].x;
                        int tmpy = particles[cumsum[j].second].y;
                        double tmptheta = particles[cumsum[j].second].theta;
                        double tmpweight = particles[cumsum[j].second].weight;
                        particle tmp(tmpx, tmpy, tmptheta, tmpweight);
                        resamples.push_back(tmp);
                    }

                    particles.clear();
                    for (int i = 0; i < resamples.size(); i++) {
                        particles.push_back(resamples[i]);
                    }

                    //          sort(particles.begin(), particles.end());
                    // Draw the object in the image (for visualisation)
                    cam.draw_object (im);
                    //          timespec fuckseiib = {2, 0};
                    //          nanosleep(&fuckseiib, NULL);
                } else { // end: if (found_landmark)

                // No observation - reset weights to uniform distribution
                for (int i = 0; i < num_particles; i++)
                    {
                        particles[i].weight = 1.0/(double)num_particles;
                    }

            }  // end: if (not found_landmark)

            // Estimate pose
            est_pose = estimate_pose (particles);

            // Visualisation
            draw_world (est_pose, particles, world);
            cvShowImage (map, world);
            cvShowImage (window, im);
        } // End: while (true)

 theend:
    // Stop the robot
    // XXX: Stop the robot

    // Clean up and return
    //cvReleaseImage (&rgb_im);
    //cvReleaseImage (&im);

    return 0;
}
