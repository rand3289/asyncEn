#pragma once
#include "geometry.h"
#include <SDL2/SDL.h>

#define WAVE_SPEED (0.1) // this is related to "moves_per_frame" in game.cpp

class Wave {
    const double waveSpeed = WAVE_SPEED;
    const double fadeSpeed = WAVE_SPEED;
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


enum WallWaveType {vertical_right, vertical_left, horisontal_up, horisontal_down};

class WallWave {
    const double waveSpeed = WAVE_SPEED;
    const double fadeSpeed = WAVE_SPEED;
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

    double getDistance(const Point2D& p) const;
    bool checkCollision(const Circle& circle) const;
    double getCollisionAngle(double angle) const;

    bool less(const WallWave& b) const { return type==b.type ? loc < b.loc : type < b.type; }
    bool operator<(const WallWave& b) const { return less(b); }
    bool equal(const WallWave& b) const { return type==b.type && loc == b.loc; }
};
