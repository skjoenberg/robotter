#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "particles.h"
#include "defines.h"
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
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */
const CvPoint landmarks [NUM_LANDMARKS] = {
    cvPoint (0, 300),
    cvPoint (0, 0),
    cvPoint (400, 300),
    cvPoint (400, 0),
};

/*
 * Colour map for drawing particles. This function determines the colour of a
 * particle from its weight.
 */
CvScalar jet (const double x) {
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
void draw_world (particle &est_pose, std::vector<particle> &particles, IplImage *im) {
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
    const CvPoint lm2 = cvPoint (landmarks[2].x+offset, landmarks[2].y+offset);
    const CvPoint lm3 = cvPoint (landmarks[3].x+offset, landmarks[3].y+offset);
    cvCircle (im, lm0, 5, CRED, 2);
    cvCircle (im, lm1, 5, CGREEN, 2);
    cvCircle (im, lm2, 5, CBLUE, 2);
    cvCircle (im, lm3, 5, CYELLOW, 2);

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
    cvPutText(im, "L1", lm0, &font, CBLACK);
    cvPutText(im, "L2", lm1, &font, CBLACK);
    cvPutText(im, "L3", lm2, &font, CBLACK);
    cvPutText(im, "L4", lm3, &font, CBLACK);

    // Draw estimated robot pose
    const CvPoint a = cvPoint ((int)est_pose.x+offset, (int)est_pose.y+offset);
    const CvPoint b = cvPoint ((int)(est_pose.x + 15.0*cos(est_pose.theta))+offset,
                               (int)(est_pose.y + 15.0*sin(est_pose.theta))+offset);
    cvCircle (im, a, 5, CMAGENTA, 2);
    cvLine   (im, a, b, CMAGENTA, 2);
}
