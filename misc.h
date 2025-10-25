#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed

// void runTests(SDL_Renderer* rend, int x, int y);

struct Point2D {
    double x,y;

    Point2D(): x(0.0), y(0.0) {}

    Point2D(double X, double Y): x(X), y(Y) {}

    Point2D translate(const Point2D& p) const { return Point2D(x+p.x, y+p.y); }

    // https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/3drota.htm
    Point2D rotate(const double a) const { // rotate around z axis
        const double sa = sin(a);
        const double ca = cos(a);
        return Point2D( x*ca-y*sa, y*ca+x*sa );
    }
};

void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void line(SDL_Renderer* rend, const Point2D& p1, const Point2D& p2);
