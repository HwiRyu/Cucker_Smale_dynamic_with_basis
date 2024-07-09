#ifndef CUCKER_SMALE_MODELING_CUCKER_SMALE_MODELING_H
#define CUCKER_SMALE_MODELING_CUCKER_SMALE_MODELING_H

#include <random>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

const int N = 100;
double lambda = 150;
const double step_size = 0.0001;

double psi_function(double s) {
    return pow(s, 1) / (1 + pow(s, 1.8));
}

double norm_distant(const double component_i[3], const double component_j[3]) {
    double dist_x = component_i[0] - component_j[0];
    double dist_y = component_i[1] - component_j[1];
    double dist_z = component_i[2] - component_j[2];
    return std::sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
}

struct Particle {
    double position[3];
    double velocity[3];
    double acceleration[3];
};


void calculate_acceleration(
        Particle *particles,
        double (*psi_function)(double),
        double (*norm_distant)(const double *, const double *)) {
    for (int i = 0; i < N; i++) {
        double sigma[3] = {0, 0, 0};
        for (int j = 0; j < N; j++) {
            if (i != j) {
                double distance = norm_distant(particles[i].position, particles[j].position);
                double psi = psi_function(distance);
                for (int k = 0; k < 3; k++) {
                    sigma[k] += psi * (particles[j].velocity[k] - particles[i].velocity[k]);
                }
            }
        }
        for (int k = 0; k < 3; k++) {
            particles[i].acceleration[k] = (lambda / N) * sigma[k];
        }
    }
}

//calculate_acceleration(particles, psi_function, norm_distant);
void runge_kutta_step(
        Particle *particles,
        double (*psi_function)(double),
        double (*norm_distant)(const double *, const double *)) {
    // Runge-Kutta 4th order method
    Particle k1[N], k2[N], k3[N], k4[N], temp_particles[N];

    // Calculate k1
    calculate_acceleration(particles, psi_function, norm_distant);
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            k1[i].velocity[k] = particles[i].velocity[k];
            k1[i].acceleration[k] = particles[i].acceleration[k];
        }
    }

    // Calculate k2
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            temp_particles[i].position[k] = particles[i].position[k] + 0.5 * step_size * k1[i].velocity[k];
            temp_particles[i].velocity[k] = particles[i].velocity[k] + 0.5 * step_size * k1[i].acceleration[k];
        }
    }
    calculate_acceleration(temp_particles, psi_function, norm_distant);
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            k2[i].velocity[k] = temp_particles[i].velocity[k];
            k2[i].acceleration[k] = temp_particles[i].acceleration[k];
        }
    }

    // Calculate k3
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            temp_particles[i].position[k] = particles[i].position[k] + 0.5 * step_size * k2[i].velocity[k];
            temp_particles[i].velocity[k] = particles[i].velocity[k] + 0.5 * step_size * k2[i].acceleration[k];
        }
    }
    calculate_acceleration(temp_particles, psi_function, norm_distant);
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            k3[i].velocity[k] = temp_particles[i].velocity[k];
            k3[i].acceleration[k] = temp_particles[i].acceleration[k];
        }
    }

    // Calculate k4
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            temp_particles[i].position[k] = particles[i].position[k] + step_size * k3[i].velocity[k];
            temp_particles[i].velocity[k] = particles[i].velocity[k] + step_size * k3[i].acceleration[k];
        }
    }
    calculate_acceleration(temp_particles, psi_function, norm_distant);
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            k4[i].velocity[k] = temp_particles[i].velocity[k];
            k4[i].acceleration[k] = temp_particles[i].acceleration[k];
        }
    }

    // Update particles
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < 3; k++) {
            particles[i].position[k] += step_size / 6.0 *
                                        (k1[i].velocity[k] + 2.0 * k2[i].velocity[k] + 2.0 * k3[i].velocity[k] +
                                         k4[i].velocity[k]);
            particles[i].velocity[k] += step_size / 6.0 * (k1[i].acceleration[k] + 2.0 * k2[i].acceleration[k] +
                                                           2.0 * k3[i].acceleration[k] + k4[i].acceleration[k]);
        }
    }
}

void step_for_time(double current_time, double initial_time, Particle *particles,
                   double (*psi_function)(double),
                   double (*norm_distant)(const double *, const double *)) {
    for (double i = initial_time; i <= current_time; i += step_size) {
        runge_kutta_step(particles, psi_function, norm_distant);
    }
}


void cucker_smale_equation(sf::RenderWindow &window, sf::VertexArray &graph, Particle *particles,
                           double size, double theta, double pi) {
    std::vector<sf::CircleShape> circles(N, sf::CircleShape(std::max(size / 20.0, 2.0)));
    for (auto &circle: circles) {
        circle.setFillColor(sf::Color::Black);
    }
    for (int i = 0; i < N; i++) {
        circles[i].setPosition(
                x_scale(rotation(particles[i].position[0], particles[i].position[1], particles[i].position[2], theta,
                                 pi).x, size),
                y_scale(rotation(particles[i].position[0], particles[i].position[1], particles[i].position[2], theta,
                                 pi).y, size));
    }
// 원형 객체 그리기
    for (const auto &circle: circles) {
        window.draw(circle);
    }
}

void change_velocity(Particle *particles, double x, double y, double z = 0) {
    double direction[3] = {0, 0, 0};
    double target[3];
    target[0] = x;
    target[1] = y;
    target[2] = z;

    for (int i = 0; i < N; i++) {
        double speed = sqrt(
                pow(particles[i].velocity[0], 2) + pow(particles[i].velocity[1], 2) + pow(particles[i].velocity[2], 2));
        for (int k = 0; k < 3; k++) {
            direction[k] = target[k] - particles[i].position[k];
        }
        double denominator = sqrt(pow(direction[0], 2) + pow(direction[1], 2) + pow(direction[2], 2));
        for (int k = 0; k < 3; k++) {
            particles[i].velocity[k] = (direction[k]  / denominator) * speed;
        }

    }

}

#endif //CUCKER_SMALE_MODELING_CUCKER_SMALE_MODELING_H
