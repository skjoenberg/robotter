#include <iostream>
#include <stdio.h>

#include "main.h"
#include "random_numbers.h"
#include "particles.h"

using namespace std;

bool debug = true;

void resample(std::vector<particle>* particles) {
    /* Lav cumsum og rand [0, 1] for at se hvilken partikel du skal duplikere */
    std::vector<particle> resamples;
    vector<pair<double, int> > cumsum;

    double cum = 0;
    int count = 0;
    for(int i = 0; i < particles->size(); i++) {
        if (particles->at(i).weight > exp(-20)) {
            cum += particles->at(i).weight;
            cumsum.push_back(pair<double, int>(cum, i));
            count++;
        }
    }

    for (int i = 0; i < NUM_PARTICLES; i++) {
        float r = randf();

        int j = 0;

        while((cumsum[j].first < r) && (j < cumsum.size())) {
            j++;
        }

        int current_x, current_y;
        current_x = particles->at(cumsum[j].second).x;
        current_y = particles->at(cumsum[j].second).y;

        double current_theta, current_weight;
        current_theta  = particles->at(cumsum[j].second).theta;
        current_weight = particles->at(cumsum[j].second).weight;

        particle current_particle(current_x, current_y, current_theta, current_weight);
        resamples.push_back(current_particle);
    }

    particles->clear();
    for (int i = 0; i < resamples.size(); i++) {
        particles->push_back(resamples[i]);
    }

    // Debugging
    if (debug) {
        cout << "Particles: " << particles->size() << "." << endl;
        cout << "Usefull particles: " << count << "." << endl;
    }
}
