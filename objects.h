#pragma once
#include "geometry.h"
#include "queue.h"
#include <SDL2/SDL.h>
#include <chrono>


enum WallWaveType{vertical_right, vertical_left, horisontal_up, horisontal_down};

class WallWave {
    const double waveSpeed = 1.0;
    const double fadeSpeed = 2.0;
    double r,g,b; // SDL_FColor color; // floating point r,g,b available since SDL 3.2.0.
    double loc;
    WallWaveType type;
public:
    WallWave(double location, WallWaveType typE): r(255), g(255), b(255), loc(location), type(typE) {}
    WallWave(const WallWave& other): r(other.r), g(other.g), b(other.b), loc(other.loc), type(other.type) {}
    WallWave(WallWave&& other) noexcept: r(other.r), g(other.g), b(other.b), loc(other.loc), type(other.type) {}
    WallWave& operator=(const WallWave& other);

    void draw(SDL_Renderer* rend, int width, int height) const;
    void move();
    double getHealth() const { return r+g+b; }
    bool checkCollision(const Circle& circle) const;
    double getCollisionAngle(double angle) const;
    bool less(const WallWave& b) const { return type==b.type ? loc < b.loc : type < b.type; }
    bool operator<(const WallWave& b) const { return less(b); }
    bool equal(const WallWave& b) const { return type==b.type && loc == b.loc; }
};


class Wave {
    const double waveSpeed = 1.0;
    const double fadeSpeed = 2.0;
    double r,g,b; // SDL_FColor color; // floating point r,g,b available since SDL 3.2.0.
public:
    Circle circle; // TODO: make this private

    Wave(): r(0), g(0), b(0), circle(0,0,0) { };
    Wave(SDL_Color rgb, double x, double y, double radius): r(rgb.r), g(rgb.g), b(rgb.b), circle(x,y,radius) {}
    Wave(const Wave& other): r(other.r), g(other.g), b(other.b), circle(other.circle) {}
    Wave(Wave&& other) noexcept: r(other.r), g(other.g), b(other.b), circle(std::move(other.circle)) {}
    Wave& operator=(const Wave& other);

    void draw(SDL_Renderer* rend) const;
    void move();
    double getHealth() const { return r+g+b; }
};


enum EventType{collision, wave, death};

struct Event {
    EventType event;
    std::chrono::high_resolution_clock::time_point time;
    double srcAngle;
};

enum ActionType{ none=0, kickLeft, kickRight, kickBoth };

struct Action {
    ActionType action;
    std::chrono::high_resolution_clock::time_point time;
};


class Life {
    double health;
    SDL_Color color;
    Point2D velocity; // TODO: this is currently unused
    double angle;
    LockFreeQueue<Action> actQ;  // brain runs on a different thread.  This could change to a network connection
    LockFreeQueue<Event> eventQ; // TODO: implement sending events to the brain
public:
    Circle circle; // TODO: make this private

    Life(): health(10.0), velocity(10,10), angle(10), circle(10,10,5) {
        color.r = color.g = color.b = 255;
        color.a = SDL_ALPHA_OPAQUE;
        circle.center.x = rand()%700; // TODO: fix this
        circle.center.y = rand()%700;
    }
    Life(const Life& other): health(other.health), color(other.color), circle(other.circle), velocity(other.velocity), angle(other.angle) {}
    Life(Life&& other) noexcept: health(other.health), color(other.color), circle(std::move(other.circle)), velocity(std::move(other.velocity)), angle(other.angle) {}
    Life& operator=(const Life& other);

    void draw(SDL_Renderer* rend) const; // draws on screen ONLY!
    void move();                   // calculates things
    double getHealth() const { return health; }

    double getAngle() const { return angle; }
    void action(const Action& a);  // agent wants to perform this action (eg: kick left, kick right)
    // TODO: accept parameters instead of a a constructed event here:
    void event(const Event& e);    // collision, wave, etc...
};
