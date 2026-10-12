#pragma once
#include "geometry.h"
#include "waves.h"
#include <SDL2/SDL.h>
//#include <unistd.h> // close()

enum ActionType{ none=0, kickLeft, kickRight, kickBoth };

enum EventType{ noop=0, collision, wave, death};

// Number of sensors arranged equally spaced around the perimeter of each Life.
// A larger number gives finer spatial (and, together with the sub-frame time
// steps in Game::draw(), finer temporal) resolution of incoming waves.
constexpr int SENSOR_COUNT = 16;

struct Event {
    int64_t time; // microseconds
    EventType event;
    int triggeredSensorNumber; // which perimeter sensor was hit (-1 for events with no sensor, e.g. death)
    RGB rgb;
};


class Life {
    constexpr static double speed = WAVE_SPEED;
    double health;
    RGB color;
    double angle;           // direction of movement
    int64_t nextWaveTimeUs; // last time this instance of Life generated a wave
    int inFd, outFd;        // agent IO file descriptors for receiving events and sending actions
public:
    Circle circle;    // TODO: make this private

    Life(int inputFd, int outputFd): health(10.0), color(), angle(10), nextWaveTimeUs(), inFd(inputFd), outFd(outputFd), circle(rand()%700, rand()%700, 5) { }
    Life(const Life& other): health(other.health), color(other.color), angle(other.angle), nextWaveTimeUs(other.nextWaveTimeUs), inFd(other.inFd), outFd(other.outFd), circle(other.circle) { }
    Life(Life&& other) noexcept: health(other.health), color(std::move(other.color)), angle(other.angle), nextWaveTimeUs(std::move(other.nextWaveTimeUs)),
        inFd(other.inFd), outFd(other.outFd), circle(std::move(other.circle)) { other.inFd = -1; other.outFd = -1; }
    // Move constructor marks the other object's handles invalid so they can be closed in destructor after 'other' object is destroyed
    // OTHERWISE DO NOT CLOSE HANDLES IN DESTRUCTOR !!!  ~Life(){ close(inFd); close(outFd); }
    Life& operator=(const Life& other);

    void draw(SDL_Renderer* rend) const; // draws on screen ONLY!
    void move(int64_t timeUs);            // calculates things
    double getHealth() const { return health; }

    double getAngle() const { return angle; }
    // TODO: accept event parameters instead of a constructed event here?
    void event(const Event& e);    // collision, wave, etc...

    bool inside(const Circle& circle) const; // for Wave
    bool checkCollision(const Circle& circle) const; // for Wave
    bool checkCollision(const WallWave& wallWave) const;
    bool checkCollision(const Life& life) const;

    // World-space position of perimeter sensor k (0 <= k < SENSOR_COUNT).
    Point2D sensorPoint(int k) const;
    // Index of the sensor whose outward direction best faces 'target'.
    int nearestSensor(const Point2D& target) const;
    // Fire an event for every sensor the wave front is currently sweeping across.
    // Near sensors are reached on earlier sub-steps than far ones, so successive
    // calls (as the wave grows) deliver events to different sensors over time.
    void senseWave(const Circle& waveFront, int64_t timeUs);
    void senseWave(const WallWave& wallWave, int64_t timeUs);
};
