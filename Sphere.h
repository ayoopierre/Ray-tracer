#ifndef SPHERE_H
#define SPHERE_H
#include "vector.h"

class Sphere{
    public:
        point3 center;
        double radius;
        color self_color;
        double smoothness;
        double transparency;
        double refractive_index;
    public:
        Sphere() {};
        Sphere(point3 c, double r, color sc, double sm, double tr, double rf): center(c), radius(r), self_color(sc), smoothness(sm), transparency(tr), refractive_index(rf) {};
        point3 get_center() const {return center;};
        double get_radius() const {return radius;};
        double get_smoothness() const {return smoothness;};
        double get_transparency() const {return transparency;};
        double get_refractive_index() const {return refractive_index;};
        color get_color() const {return self_color;};
};

#endif