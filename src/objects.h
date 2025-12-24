#pragma once
#include "geometry.h"
#include <SDL2/SDL.h>
#include <unistd.h> // close()
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point Time;


enum WallWaveType{vertical_right, vertical_left, horisontal_up, horisontal_down};

class WallWave {
    const double waveSpeed = 2.0;
    const double fadeSpeed = 2.0;
    RGB rgb;
    double loc;
    WallWaveType type;
public:
    WallWave(double location, WallWaveType typE): rgb(255,255,255), loc(location), type(typE) {}
    WallWave(const WallWave& other): rgb(other.rgb), loc(other.loc), type(other.type) {}
    WallWave(WallWave&& other) noexcept: rgb(other.rgb), loc(other.loc), type(other.type) {}
    WallWave& operator=(const WallWave& other);

    void draw(SDL_Renderer* rend, int width, int height) const;
    void move();
    double getHealth() const { return rgb.r + rgb.g + rgb.b; }
    const RGB& getRGB() const { return rgb; }

    bool checkCollision(const Circle& circle) const;
    double getCollisionAngle(double angle) const;
    double getDistance(const Point2D& p) const;

    bool less(const WallWave& b) const { return type==b.type ? loc < b.loc : type < b.type; }
    bool operator<(const WallWave& b) const { return less(b); }
    bool equal(const WallWave& b) const { return type==b.type && loc == b.loc; }
};


class Wave {
    const double waveSpeed = 2.0;
    const double fadeSpeed = 2.0;
    RGB rgb; // SDL_FColor color floating point r,g,b available since SDL 3.2.0.
public:
    Circle circle; // TODO: make this private

    Wave(): rgb(0,0,0), circle(0,0,0) { };
    Wave(const RGB& rgbp, double x, double y, double radius): rgb(rgbp), circle(x,y,radius) {}
    Wave(const Wave& other): rgb(other.rgb), circle(other.circle) {}
    Wave(Wave&& other) noexcept: rgb(std::move(other.rgb)), circle(std::move(other.circle)) {}
    Wave& operator=(const Wave& other);

    void draw(SDL_Renderer* rend) const;
    void move();
    double getHealth() const { return rgb.r + rgb.g + rgb.b; }
    const RGB& getRGB() const { return rgb; }
};


enum ActionType{ none=0, kickLeft, kickRight, kickBoth };

enum EventType{ noop=0, collision, wave, death};

struct Event {
    int64_t time; // microseconds
    EventType event;
    double srcAngle;
    int triggeredSensorNumber;
    RGB rgb;
};


class Life {
    const double speed = 1.0;
    double health;
    RGB color;
    double angle;     // direction of movement
    Time lastWave;    // last time this instance of Life generated a wave
    int inFd, outFd;  // agent IO file descriptors for receiving events and sending actions
public:
    Circle circle;    // TODO: make this private

    Life(int inputFd, int outputFd): health(10.0), color(), angle(10), lastWave(), inFd(inputFd), outFd(outputFd), circle(rand()%700, rand()%700, 5) { }
    Life(const Life& other): health(other.health), color(other.color), angle(other.angle), lastWave(other.lastWave), inFd(other.inFd), outFd(other.outFd), circle(other.circle) { }
    Life(Life&& other) noexcept: health(other.health), color(std::move(other.color)), angle(other.angle), lastWave(std::move(other.lastWave)),
        inFd(other.inFd), outFd(other.outFd), circle(std::move(other.circle)) { other.inFd = -1; other.outFd = -1; }
    // Move constructor marks the other object's handles invalid so they can be closed in destructor after 'other' object is destroyed
    // OTHERWISE DO NOT CLOSE HANDLES IN DESTRUCTOR !!!  ~Life(){ close(inFd); close(outFd); }
    Life& operator=(const Life& other);

    void draw(SDL_Renderer* rend) const; // draws on screen ONLY!
    void move(const Time& t);            // calculates things
    double getHealth() const { return health; }

    double getAngle() const { return angle; }
    // TODO: accept event parameters instead of a constructed event here?
    void event(const Event& e);    // collision, wave, etc...
};
