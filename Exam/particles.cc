#include <vector>
#include "particles.h"
#include "random_numbers.h"
#include <math.h>
#include <iostream>
#include <defines.h>


using namespace std;

void particles_init(std::vector<particle> &particles) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
        particles[i].x = 2000.0*randf() - 1000;
        particles[i].y = 2000.0*randf() - 1000;
        particles[i].theta = 2.0*M_PI*randf() - M_PI;
        particles[i].weight = 1.0/(double)NUM_PARTICLES;
    }
}

particle estimate_pose (std::vector<particle> &particles)
{
    double x_sum = 0, y_sum = 0, cos_sum = 0, sin_sum = 0;
    const int len = particles.size ();
    for (int i = 0; i < len; i++)
      {
        x_sum += particles[i].x;
        y_sum += particles[i].y;
        cos_sum += cos (particles[i].theta);
        sin_sum += sin (particles[i].theta);
      }
    const double flen = (double)len;
    const double x = x_sum/flen;
    const double y = y_sum/flen;
    const double theta = atan2 (sin_sum/flen, cos_sum/flen);
    return particle (x, y, theta);
}

void move_particles(std::vector<particle> &particles, double delta_x, double delta_y, double delta_theta) {
    for(int i = 0; i < particles.size(); i++) {
        move_particle(particles[i], (delta_x * 100), (delta_x * 100), delta_theta);
    }
}

// XXX: You implement this
void move_particle (particle &p, double delta_x, double delta_y, double delta_theta)
{
    p.x += delta_x;
    p.y += delta_y;
    p.theta += delta_theta;
    if (p.theta > M_PI) {
        p.theta -= 2 * M_PI;
    } else if (p.theta < -M_PI) {
        p.theta += 2 * M_PI;
    }
}



void add_uncertainty (std::vector<particle> &particles, double sigma, double theta_kappa)
{
    const int len = particles.size();
    for (int i = 0; i < len; i++)
      {
        particles[i].x += randn (0, sigma);
        particles[i].y += randn (0, sigma);
        particles[i].theta += randn (0, theta_kappa);
        if (particles[i].theta > M_PI) {
            particles[i].theta -= 2 * M_PI;
        } else if (particles[i].theta < -M_PI) {
            particles[i].theta += 2 * M_PI;
        }
        //   particles[i].theta = fmod(rand_von_mises (particles[i].theta, theta_kappa), 2.0f * M_PI) - M_PI;
      }

}
