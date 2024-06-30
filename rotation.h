#ifndef INC_3D_GRAPH_MODELING_BASIS_ROTATION_H
#define INC_3D_GRAPH_MODELING_BASIS_ROTATION_H
#include <cmath>

struct view_point {
    double x;
    double y;
};

view_point rotation(double x, double y, double z, double theta, double pi) {
    view_point rotated_point;
    rotated_point.x = -sin(theta) * x  + cos(theta) * y;
    rotated_point.y = -sin(pi) * cos(theta) * x - sin(pi) * sin(theta) * y + cos(pi) * z;
    return rotated_point;
}

view_point backtracking(double x, double y,double theta, double pi) {
    view_point origin_point;
    origin_point.x = -x * sin(theta) - cos(theta) * y / sin(pi);
    origin_point.y = x * cos(theta) - sin(theta) * y / sin(pi);
    return origin_point;
}

#endif //INC_3D_GRAPH_MODELING_BASIS_ROTATION_H
