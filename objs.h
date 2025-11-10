#pragma once
#include "misc.h"
#include <SDL2/SDL.h>
#include <chrono>
#include "lockfree_queue.hpp"

// every time Life kicks, it creates a wave
// waves die out when their color fades to zero
struct Wave {
    const double waveSpeed = 1.0;
    const double fadeSpeed = 2.0;

    double r,g,b; // SDL_FColor color; // floating point r,g,b available since SDL 3.2.0.
    Circle circle;

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

enum ActionType{ kickLeft, kickRight, kickBoth };

struct Action {
    std::chrono::high_resolution_clock::time_point time;
    ActionType action;
};

struct Life {
    double health;
    SDL_Color color;
    Circle circle;
    Point2D velocity;
    double angle;
    LockFreeQueue<Action> actQ; // agent's brain runs on a different thread

    Life(): health(10.0), circle(100,100,10), velocity(10,10), angle(0) { 
        color.r = color.g = color.b = 255;
        color.a = SDL_ALPHA_OPAQUE;
    }
    Life(const Life& other): health(other.health), color(other.color), circle(other.circle), velocity(other.velocity), angle(other.angle) {}
    Life(Life&& other) noexcept: health(other.health), color(other.color), circle(std::move(other.circle)), velocity(std::move(other.velocity)), angle(other.angle) {}
    Life& operator=(const Life& other);

    void draw(SDL_Renderer* rend); // draws on screen ONLY!
    void move();                   // calculates position, velocity, orientation
    void action(const Action& a);  // agent wants to perform this action (eg: kick left, kick right)
    void event(const Event& e);    // collision, wave, health increase or decrease.
};

// It seems ALL objects can calculate their own locations, velocity and acceleration till a collision occurs.
// All objects can use this information to draw itself.
// When a collision occurs, location, velocity and acceleration have to be calculated and recorded

// Walls need to send linear waves