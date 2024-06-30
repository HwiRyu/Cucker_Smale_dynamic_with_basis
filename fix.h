
#include <SFML/Graphics.hpp>

void point_fix(sf::RenderWindow &window,
               sf::View &view,
               double x, double y, double z,
               double size, double theta, double pi) {

    double v_x = x_scale(rotation(x,y,z, theta, pi).x, size);
    double v_y = y_scale(rotation(x,y,z, theta, pi).y, size);

    view.setCenter(v_x , v_y );
    window.setView(view);
}

