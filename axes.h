#include <SFML/Graphics.hpp>
#include <cmath>

void zero_plane(sf::RenderWindow &window,
                sf::VertexArray &graph,
                double size,
                double view_center_x,
                double view_center_y,
                double theta,
                double pi
) {
    sf::Color custom = sf::Color(0, 0, 0, 100);
    double length = 1;
    view_center_x -= fmod(view_center_x, length);
    view_center_y -= fmod(view_center_y, length);

    double x_start = view_center_x - 10;
    double x_end = view_center_x + 10;
    double y_start = view_center_y - 10;
    double y_end = view_center_y + 10;

    for (double x = x_start; x <= x_end; x += length) {
        for (double y = y_start; y <= y_end; y += length) {
            graph.append(sf::Vertex(sf::Vector2f(
                                            x_scale(rotation(x, y, 0, theta, pi).x, size),
                                            y_scale(rotation(x, y, 0, theta, pi).y, size)),
                                    custom));
        }
        window.draw(graph);
        graph.clear();
    }

    for (double y = y_start; y <= y_end; y += length) {
        for (double x = x_start; x <= x_end; x += length) {
            graph.append(sf::Vertex(sf::Vector2f(
                                            x_scale(rotation(x, y, 0, theta, pi).x, size),
                                            y_scale(rotation(x, y, 0, theta, pi).y, size)),
                                    custom));
        }


        window.draw(graph);
        graph.clear();

        if (view_center_x<= 10 && view_center_x >= -10 && view_center_y<= 10 && view_center_y >= -10) {
            graph.append(sf::Vertex(sf::Vector2f(
                                            x_scale(rotation(0, 0, 0, theta, pi).x, size),
                                            y_scale(rotation(0, 0, 0, theta, pi).y, size)),
                                    custom));
            graph.append(sf::Vertex(sf::Vector2f(
                                            x_scale(rotation(0, 0, 10, theta, pi).x, size),
                                            y_scale(rotation(0, 0, 10, theta, pi).y, size)),
                                    custom));
            window.draw(graph);
            graph.clear();
        }

    }
}
