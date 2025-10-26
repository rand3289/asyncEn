#include "misc.h"
#include <SDL2/SDL.h>


// every time Life kicks, it creates a wave
// waves die out when their color fades to zero
struct Wave {
    const double waveSpeed = 1.0;
    const double fadeSpeed = 2.0;

    double r,g,b; // SDL_FColor color; // floating point r,g,b available since SDL 3.2.0.
    Circle circle;
    Wave(SDL_Color rgb, double x, double y, double radius): r(rgb.r), g(rgb.g), b(rgb.b), circle(x,y,radius) {}

    void draw(SDL_Renderer* rend) {
        SDL_SetRenderDrawColor(rend, r, g, b, SDL_ALPHA_OPAQUE);
        circle.draw(rend);
    }

    void move() {
        circle.radius += waveSpeed;
        r = r>fadeSpeed ? r-fadeSpeed : 0.0;
        g = g>fadeSpeed ? g-fadeSpeed : 0.0;
        b = b>fadeSpeed ? b-fadeSpeed : 0.0;
    }

    bool isGone() {
        return r<=0 && g<=0 && b<=0;
    }
};


struct Life {
    double health;
    SDL_Color color;
    Circle circle;
    Point2D velocity;
    double angle;
    void draw(SDL_Renderer* rend);
    void kick(bool leftLeg, bool rightLeg); // action
    void hitEvent();
    void rippleEvent();
};
