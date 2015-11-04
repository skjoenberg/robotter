#include <iostream>
#include <stdio.h>

#include "defines.h"
#include "random_numbers.h"
#include "particles.h"

using namespace std;

void resample(vector<particle> &particles, int box_x, int box_y, int measured_angle, int measured_distance) {
    // Debugging
    bool debug = false;

    // Give particles weights
    double sum = 0;
    for (int i = 0; i < particles.size(); i++) {
        // Measure euclidean distance to landmark
        double deltax, deltay;
        deltax = particles[i].x - box_x;
        deltay = particles[i].y - box_y;

        // Euclidean distance to box
        double dist;
        dist = sqrt(pow(deltax, 2.0) + pow(deltay, 2.0));
        if (dist == 0) {
            cout << "ALARM ALARM ALARM ALARM" << endl;
            cout << "ALARM ALARM ALARM ALARM" << endl;
            cout << "ALARM ALARM ALARM ALARM" << endl;
            cout << "ALARM ALARM ALARM ALARM" << endl;
            cout << "ALARM ALARM ALARM ALARM" << endl;
            cout << "ALARM ALARM ALARM ALARM" << endl;

        }

        // Angle between particle and box
        double angletobox;
        angletobox = atan(deltay / deltax);

        // If deltax > 0, then the angle needs to be turned by a half circle.
        if (deltax > 0) {
            angletobox -= M_PI;
        }

        // Difference in angle
        double deltaangle;
        deltaangle = particles[i].theta - angletobox;

        // The angles are between (-pi, pi)
        if (deltaangle > M_PI){
            deltaangle -= 2 * M_PI;
        } else if (deltaangle < -M_PI) {
            deltaangle += 2 * M_PI;
        }

        // Calculate weight of the current particle
        double gaussman = 1. / sqrt(2. * M_PI * pow(SIGMA, 2.));
        double angleweight, distweight;
        angleweight = gaussman * exp(-((pow(measured_angle - deltaangle, 2.0) / (2.0 * pow(SIGMA_THETA, 2.0)))));
        distweight = gaussman * exp(-((pow(measured_distance - dist, 2.0) / (2.0 * pow(SIGMA, 2.0)))));

        double tmpweight;
        tmpweight = angleweight*distweight;

        // Add the weight to a sum (used later on to normalize weights)
        sum += tmpweight;

        // Save the weight in a particle array
        particles[i].weight = tmpweight;
    }

    // Normalize weights
    for (int i = 0; i < particles.size(); i++) {
        particles[i].weight = particles[i].weight / sum;
    }

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
    if (cumsum.size() > 0) {
        for (int i = 0; i < NUM_PARTICLES; i++) {
            float r = randf();

            int j = 0;

            while((j < cumsum.size()) && (cumsum[j].first < r)) {
                j++;
            }

            int current_x, current_y;
            current_x = particles[cumsum[j].second].x;
            current_y = particles[cumsum[j].second].y;

            double current_theta, current_weight;
            current_theta  = particles[cumsum[j].second].theta;
            current_weight = particles[cumsum[j].second].weight;

            particle current_particle(current_x, current_y, current_theta, current_weight);
            resamples.push_back(current_particle);
        }


        particles.clear();
        for (int i = 0; i < resamples.size(); i++) {
            particles.push_back(resamples[i]);
        }
    }

    // Debugging
    if (debug) {
        cout << "Particles: " << particles.size() << "." << endl;
        cout << "Usefull particles: " << count << "." << endl;
    }
}
