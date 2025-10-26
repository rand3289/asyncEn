#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed


struct Point2D {
    double x,y;
    Point2D(): x(0.0), y(0.0) {}
    Point2D(double X, double Y): x(X), y(Y) {}
    
    // Copy constructor
    Point2D(const Point2D& other): x(other.x), y(other.y) {}
    
    // Move constructor
    Point2D(Point2D&& other) noexcept: x(other.x), y(other.y) {}
    
    // Assignment operator
    Point2D& operator=(const Point2D& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    Point2D translate(const Point2D& p) const { return Point2D(x+p.x, y+p.y); }

    // https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/3drota.htm
    Point2D rotate(const double a) const { // rotate around z axis
        const double sa = sin(a);
        const double ca = cos(a);
        return Point2D( x*ca-y*sa, y*ca+x*sa );
    }

    double distance(const Point2D& rhs) const {
        double dx = x - rhs.x;
        double dy = y - rhs.y;
        return sqrt(dx*dx + dy*dy);
    }
};


void line(SDL_Renderer* rend, const Point2D& p1, const Point2D& p2);
void drawCircle(SDL_Renderer* rend, int32_t centreX, int32_t centreY, int32_t radius);


struct Circle {
    Point2D center;
    double radius;
    Circle(double x, double y, double rad): center(x,y), radius(rad) {}
    
    // Copy constructor
    Circle(const Circle& other): center(other.center), radius(other.radius) {}
    
    // Move constructor
    Circle(Circle&& other) noexcept: center(std::move(other.center)), radius(other.radius) {}
    
    // Assignment operator
    Circle& operator=(const Circle& other) {
        if (this != &other) {
            center = other.center;
            radius = other.radius;
        }
        return *this;
    }
    
    void draw(SDL_Renderer* rend){ drawCircle(rend,center.x, center.y, radius); }
    double distance(Circle& rhs){
        return center.distance(rhs.center) - (radius+rhs.radius);
    }
};
