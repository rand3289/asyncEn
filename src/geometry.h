#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <cmath>

#define RADIAN (180.0/M_PI) // degrees in one radian


struct RGB{
    double r,g,b;
    RGB(): r(0.0),g(0.0),b(0.0) {}
    RGB(double R, double G, double B): r(R), g(G), b(B) {}
    RGB(const RGB& rgb): r(rgb.r), g(rgb.g), b(rgb.b) {}
    RGB(RGB&& rgb) noexcept: r(rgb.r), g(rgb.g), b(rgb.b) {}
    RGB& operator=(const RGB& rgb){ r=rgb.r; g=rgb.g; b=rgb.b; return *this; }
};


struct Point2D {
    double x,y;
    Point2D(): x(0.0), y(0.0) {}
    Point2D(double X, double Y): x(X), y(Y) {}
    Point2D(const Point2D& other): x(other.x), y(other.y) {}
    Point2D(Point2D&& other) noexcept: x(other.x), y(other.y) {}

    Point2D& operator=(const Point2D& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    Point2D translate(const Point2D& p) const { return Point2D(x+p.x, y+p.y); }

    // https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/3drota.htm
    Point2D rotate(double a) const { // rotate around z axis
        a /= RADIAN; // convert to radians
        const double sina = sin(a);
        const double cosa = cos(a);
        return Point2D( x*cosa - y*sina, y*cosa + x*sina );
    }

    double angle(const Point2D& rhs) const {
        return  RADIAN * atan2(y-rhs.y, x-rhs.x);
    }

    double distance(const Point2D& rhs) const {
        double dx = x - rhs.x;
        double dy = y - rhs.y;
        return sqrt(dx*dx + dy*dy);
    }
};


inline void line(SDL_Renderer* rend, const Point2D& p1, const Point2D& p2){
    SDL_RenderDrawLine(rend, p1.x, p1.y , p2.x, p2.y);
}


void drawCircle(SDL_Renderer* rend, int32_t centreX, int32_t centreY, int32_t radius);


struct Circle {
    Point2D center;
    double radius;
    Circle(double x, double y, double rad): center(x,y), radius(rad) {}
    Circle(const Circle& other): center(other.center), radius(other.radius) {}
    Circle(Circle&& other) noexcept: center(std::move(other.center)), radius(other.radius) {}

    Circle& operator=(const Circle& other) {
        center = other.center;
        radius = other.radius;
        return *this;
    }

    void draw(SDL_Renderer* rend) const {
        drawCircle(rend, center.x, center.y, radius);
    }

    // distance between CIRCUMFERENCES of two circles (not centers)
    double distance(const Circle& b) const {
        return center.distance(b.center) - (radius+b.radius);
    }

    bool checkCollision(const Circle& b) const {
        return distance(b) <= 0;
    }

    bool inside(const Circle& b) const {
        return distance(b) <= (radius - b.radius);
    }

    // push two circles apart after a collision
    void pushApart(Circle& b) {
        double angle = center.angle(b.center);  // angle between two circle centers
        double delta = abs(distance(b) / 1.99); // move each Circle just over half the distance away
        Point2D moveBy = Point2D(delta, 0).rotate(angle);
        center = center.translate(moveBy);
        moveBy = moveBy.rotate(180); // push the second circle the other way
        b.center = b.center.translate(moveBy);
    }
};
