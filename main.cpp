//필요한 기능
//1. 드래그로 화면 이동
//2. 현재 보고있는 수직선을 기준으로 화면 회전
//3. 현재 보고있는 (x,y)를 기준으로 화면 확대
//4. metal을 사용한 gpu 가속
//5. 시간 조정

const int width = 1500;
const int height = 900;


#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>

#include "rotation.h"
#include "scale.h"
#include "function.h"
#include "draw.h"
#include "axes.h"
#include "fix.h"
#include "cucker_smale_modeling.h"

Particle particles[N];

int main() {
    sf::RenderWindow window(sf::VideoMode(width, height), "CSmodel");

    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Monaco.ttf")) {
        std::cerr << "Error loading font\n";
        return 1;
    }
    sf::VertexArray one(sf::LineStrip);
    sf::VertexArray two(sf::LineStrip);

    sf::View graphView(sf::FloatRect(-width / 2, -height / 2, width, height));
    sf::View initial_View(sf::FloatRect(-width / 2, -height / 2, width, height));

    window.setView(graphView);

    sf::Text text1("", font, 12);
    text1.setFillColor(sf::Color::Black);

    sf::Text time_text("", font, 12);
    time_text.setFillColor(sf::Color::Black);

    sf::Text max_text("", font, 12);
    max_text.setFillColor(sf::Color::Black);

    sf::Text click_text("", font, 12);
    click_text.setFillColor(sf::Color::Black);


    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> pos_dist(-50, 50); // 위치 범위
    std::uniform_real_distribution<double> vel_dist(-1000.0, 1000.0);   // 속도 범위

    for (int i = 0; i < N; i++) {
        particles[i].position[0] = pos_dist(eng);
        particles[i].position[1] = pos_dist(eng);
        particles[i].position[2] = pos_dist(eng);

        particles[i].velocity[0] = vel_dist(eng);
        particles[i].velocity[1] = vel_dist(eng);
        particles[i].velocity[2] = vel_dist(eng);
    }


    double size = 30;
    double x_angle = -10;
    double y_angle = 5.5;
    const double Pi = 3.14159265358979;
    bool domain_fixed = true;
    double current_center_x;
    double current_center_y;
    int delta_size = 5;
    double moving_epsilon = 0;
    double t = 0;
    sf::Time elapsedTime;
    float time_swith = 0.0001;
    bool time_change = false;
    bool center_fixed = false;
    double click_x, click_y;
    bool click = false;

    sf::Clock clock;
    sf::Clock clock_t;

    sf::Time lastClickTime = sf::Time::Zero;
    sf::Vector2i lastMousePos;
    sf::Vector2f startPoint;

    while (window.isOpen()) {

        elapsedTime = clock_t.getElapsedTime();
        if (elapsedTime.asSeconds() >= time_swith) {
            t += time_swith;
            clock_t.restart();
            time_change = true;
        }


        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up:
                        y_angle -= 1;
                        break;
                    case sf::Keyboard::Down:
                        y_angle += 1;
                        break;
                    case sf::Keyboard::Left:
                        x_angle += 1;
                        break;
                    case sf::Keyboard::Right:
                        x_angle -= 1;
                        break;
                    case sf::Keyboard::R:
                        for (int i = 0; i < N; i++) {
                            particles[i].velocity[0] = vel_dist(eng);
                            particles[i].velocity[1] = vel_dist(eng);
                            particles[i].velocity[2] = vel_dist(eng);
                        }
                        break;
                    case sf::Keyboard::F:
                        if (center_fixed){
                            center_fixed = false;
                        }
                        else {
                            center_fixed = true;
                        }
                        break;
                    case sf::Keyboard::W:
                        if (size <= 200) {
                            graphView.move(sf::Vector2f((graphView.getCenter().x / size) * delta_size,
                                                        (graphView.getCenter().y / size) * delta_size));
                            size += delta_size;
                            window.setView(graphView);
                        }
                        break;
                    case sf::Keyboard::S:
                        if (size > 5) {
                            graphView.move(sf::Vector2f((-graphView.getCenter().x / size) * delta_size,
                                                        (-graphView.getCenter().y / size) * delta_size));

                            size -= delta_size;
                            window.setView(graphView);
                        }
                        break;
                    default:
                        break;
                }
            }
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Time currentTime = clock.getElapsedTime();
                lastMousePos = sf::Mouse::getPosition(window);
                startPoint = window.mapPixelToCoords(lastMousePos); // 초기 클릭 위치 저장

                if ((t > 0.0001) & (currentTime - lastClickTime <= sf::seconds(0.1))) {
                    click = true;
                }

                lastClickTime = currentTime;
            }

            if (event.type == sf::Event::MouseMoved) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i newMousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f delta = window.mapPixelToCoords(newMousePos) - window.mapPixelToCoords(lastMousePos);
                    graphView.move(-delta); // 시야 이동
                    window.setView(graphView); // 뷰 설정 업데이트
                    lastMousePos = newMousePos; // 새로운 마우스 위치 저장

                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        // theta, pi에 관한 파트 ////////////////////////////////////////////////////////////////////////////////
        double pi = Pi * y_angle / 120;      //라디안 값
        double theta = Pi * (x_angle / 120);
        pi = std::fmod(pi, 2 * Pi);
        theta = std::fmod(theta, 2 * Pi);

        if (pi < 0) {
            pi += 2 * Pi;
        }
        if (theta < 0) {
            theta += 2 * Pi;
        }

        double max_velocity_difference = 0.0;

        for (int i = 0; i < N; ++i) {
            for (int j = i + 1; j < N; ++j) {
                double diff_x = particles[i].velocity[0] - particles[j].velocity[0];
                double diff_y = particles[i].velocity[1] - particles[j].velocity[1];
                double diff_z = particles[i].velocity[2] - particles[j].velocity[2];
                double velocity_diff = std::sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
                max_velocity_difference = std::max(max_velocity_difference, velocity_diff);
            }
        }

        double center_particles[3] = {0, 0, 0};

        for (int k = 0; k < 3; ++k) {
            for (int i = 0; i < N; ++i) {
                center_particles[k] += particles[i].position[k];
            }
            center_particles[k] /= N;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        if (center_fixed) {
            point_fix(window, graphView,
                      center_particles[0], center_particles[1], center_particles[2],
                      size, theta, pi);
        }
        if (click){
            click_x = x_scale(startPoint.x, 1/size);
            click_y = y_scale(startPoint.y, 1/size);

            auto result = backtracking(click_x, click_y, theta, pi);
            click_x = result.x;
            click_y = result.y;

            change_velocity(particles, click_x, click_y, 0);
            click = false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////



        double view_center_x = graphView.getCenter().x / (size);
        double view_center_y = -graphView.getCenter().y / (size);
        double center_x = backtracking(view_center_x, view_center_y, theta, pi).x;
        double center_y = backtracking(view_center_x, view_center_y, theta, pi).y;

        if (0.005 * Pi < pi && pi < 1.995 * Pi) {
            current_center_x = center_x;
            current_center_y = center_y;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        if (time_change) {
            step_for_time(t, t - time_swith, particles, psi_function, norm_distant);
            time_change = false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        function_view(window, one, size, one_variable_function, current_center_x, current_center_y, theta, pi);
        zero_plane(window, one, size, current_center_x, current_center_y, theta, pi);
        cucker_smale_equation(window, two, particles, size, theta, pi);

        ////////////////////////////////////////////////////////////
        text1.setString("x:" + std::to_string(center_x) + "y" + std::to_string(center_y));
        text1.setPosition(graphView.getCenter().x + width / 2 - 180, graphView.getCenter().y - height / 2 + 50);
        window.draw(text1);

        time_text.setString("time: " + std::to_string(t));
        time_text.setPosition(graphView.getCenter().x + width / 2 - 180, graphView.getCenter().y - height / 2 + 70);
        window.draw(time_text);

        max_text.setString("Max: " + std::to_string(max_velocity_difference));
        max_text.setPosition(graphView.getCenter().x + width / 2 - 180, graphView.getCenter().y - height / 2 + 90);
        window.draw(max_text);


        click_text.setString("click_x:" + std::to_string(click_x) + "click_x" + std::to_string(click_y));
        click_text.setPosition(graphView.getCenter().x + width / 2 - 300, graphView.getCenter().y - height / 2 + 110);
        window.draw(click_text);

        window.display();

    }

    return 0;
}