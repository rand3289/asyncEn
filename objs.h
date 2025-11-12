#pragma once
#include "misc.h"
#include "queue.h"
#include <SDL2/SDL.h>
#include <chrono>


class Wave {
    const double waveSpeed = 1.0;
    const double fadeSpeed = 2.0;

    double r,g,b; // SDL_FColor color; // floating point r,g,b available since SDL 3.2.0.
public:
    Circle circle; // TODO: make this private

    Wave(SDL_Color rgb, double x, double y, double radius): r(rgb.r), g(rgb.g), b(rgb.b), circle(x,y,radius) {}
    Wave(const Wave& other): r(other.r), g(other.g), b(other.b), circle(other.circle) {}
    Wave(Wave&& other) noexcept: r(other.r), g(other.g), b(other.b), circle(std::move(other.circle)) {}

    Wave& operator=(const Wave& other);
    void draw(SDL_Renderer* rend);
    void move();
    bool isGone() { return r<=0 && g<=0 && b<=0; }
};


enum EventType{collision, wave};

struct Event {
    std::chrono::high_resolution_clock::time_point time;
    EventType event;
    double srcAngle;
};

enum ActionType{ none=0, kickLeft, kickRight, kickBoth };

struct Action {
    std::chrono::high_resolution_clock::time_point time;
    ActionType action;
};

class Life {
    double health;
    SDL_Color color;
    Point2D velocity;
    double angle;
    LockFreeQueue<Action> actQ; // agent's brain runs on a different thread
public:
    Circle circle; // TODO: make this private

    Life(): health(10.0), circle(10,10,10), velocity(10,10), angle(10) { 
        color.r = color.g = color.b = 255;
        color.a = SDL_ALPHA_OPAQUE;
// TODO: debugging:
        circle.center.x = rand()%700;
        circle.center.y = rand()%700;
    }
    Life(const Life& other): health(other.health), color(other.color), circle(other.circle), velocity(other.velocity), angle(other.angle) {}
    Life(Life&& other) noexcept: health(other.health), color(other.color), circle(std::move(other.circle)), velocity(std::move(other.velocity)), angle(other.angle) {}
    Life& operator=(const Life& other);

    void draw(SDL_Renderer* rend); // draws on screen ONLY!
    void move();                   // calculates position, velocity, orientation
    void action(const Action& a);  // agent wants to perform this action (eg: kick left, kick right)
    void event(const Event& e);    // collision, wave, health increase or decrease.
    double getHealth(){ return health; }
};
