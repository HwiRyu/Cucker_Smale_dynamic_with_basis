#include <SFML/Graphics.hpp>


void function_view(sf::RenderWindow &window,
                         sf::VertexArray &graph,
                         double size,
                         double function(double, double),
                         double view_center_x,
                         double view_center_y,
                         double theta,
                         double pi
) {
    sf::Color custom = sf::Color(0, 0, 255, 100);
    double length = 0.1;
    double range = 8;
    double x_start = view_center_x - range;
    double x_end = view_center_x + range;
    double y_start = view_center_y - range;
    double y_end = view_center_y + range;

    for (double x = x_start; x <= x_end; x += length) {
        for (double y = y_start; y <= y_end; y += length) {
            double z = function(x,y);
            graph.append(sf::Vertex(sf::Vector2f(
                    x_scale(rotation(x,y,z,theta, pi).x, size),
                    y_scale(rotation(x,y,z,theta, pi).y, size)),
                    custom));
        }
        window.draw(graph);
        graph.clear();
    }

    for (double y = y_start; y <= y_end; y += length) {
        for (double x = x_start; x <= x_end; x += length) {
            double z = function(x,y);

            graph.append(sf::Vertex(sf::Vector2f(
                    x_scale(rotation(x,y,z,theta, pi).x, size),
                    y_scale(rotation(x,y,z,theta, pi).y, size)),
                    custom));
        }
        window.draw(graph);
        graph.clear();
    }
}
