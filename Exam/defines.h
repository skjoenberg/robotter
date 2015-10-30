#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#define NUM_PARTICLES 2000

#define SIGMA 20.0
#define SIGMA_THETA 0.1


const CvPoint landmarks [NUM_LANDMARKS] = {
    cvPoint (0, 300),
    cvPoint (0, 0),
    cvPoint (400, 300),
    cvPoint (400, 0)
};

 

#define NUM_LANDMARKS 4
