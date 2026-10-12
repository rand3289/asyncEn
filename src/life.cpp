#include "life.h"
#include "game.h"   // Game::getInstance()
#include <unistd.h> // read()
#include <cmath>    // std::fabs()
#include <iostream> // for debugging


void Life::draw(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

// writes Event into inFd
void Life::event(const Event& e){
    int size;
    char buff[32];
    switch(e.event){
        case EventType::wave:
            size = sprintf(buff, "1,%lld,%d\n", e.time, e.triggeredSensorNumber);
            break;
        case EventType::collision:
            size = sprintf(buff, "2,%lld,%d\n", e.time, e.triggeredSensorNumber);
            break;
        case EventType::death:
            size = sprintf(buff, "3,%lld,0\n", e.time);
            break;
        default: return;
    }
    write(inFd,buff,size);
}

void Life::move(int64_t timeUs){
    static const Point2D unitVector(speed, 0);

    char buff[128];
    ssize_t rd = read(outFd, buff, sizeof(buff)-1); // ssize_t: read() returns -1 on EAGAIN/error
    if( rd > 0 ){
        buff[rd] = 0;
        int time, action, param;
        if(3 != sscanf(buff, "%d,%d,%d\n", &time, &action, &param)){
            return;
        }
        switch(action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth:  break;
            case ActionType::none:      return;
            default:
                std::cout << "\nUnknown command from agent: " << buff << std::endl;
                std::cout.flush();
                return;
        }
        // create a "unit vector", rotate it by angle, translate position using unit vector
        circle.center = circle.center.translate( unitVector.rotate(angle) );

        if( timeUs > nextWaveTimeUs ){
            static Game& game = Game::getInstance();
            game.addWave(color, circle.center);    // every movement kicks up a wave
            constexpr int64_t halfSecond = 500000; // in microseconds
            nextWaveTimeUs = timeUs+halfSecond;
        }
    }
}

Life& Life::operator=(const Life& other) {
    if (this != &other) {
        health = other.health;
        color = other.color;
        angle = other.angle;
        nextWaveTimeUs = other.nextWaveTimeUs;
        inFd = other.inFd;
        outFd = other.outFd;
        circle = other.circle;
    }
    return *this;
}


bool Life::inside(const Circle& c) const { return c.inside(circle); }
bool Life::checkCollision(const Circle& c)const { return c.checkCollision(circle); }
bool Life::checkCollision(const WallWave& wallWave) const { return wallWave.checkCollision(circle); }
bool Life::checkCollision(const Life& life) const { return life.checkCollision(circle); }


// World-space position of perimeter sensor k, equally spaced around the circle.
Point2D Life::sensorPoint(int k) const {
    const double a = (360.0 * k) / SENSOR_COUNT; // degrees
    // place a radius-length vector pointing right, then rotate it to the sensor angle
    return circle.center.translate( Point2D(circle.radius, 0).rotate(a) );
}

// Pick the sensor whose outward direction most closely faces 'target'.
int Life::nearestSensor(const Point2D& target) const {
    int best = 0;
    double bestDist = sensorPoint(0).distance(target);
    for(int k = 1; k < SENSOR_COUNT; ++k){
        const double d = sensorPoint(k).distance(target);
        if(d < bestDist){
            bestDist = d;
            best = k;
        }
    }
    return best;
}

// A circular wave front is an expanding ring of radius waveFront.radius centred
// at waveFront.center.  The ring grows by WAVE_SPEED each sub-step, so a sensor
// is swept on the sub-step when the front crosses that sensor's distance from the
// wave centre: the front is at (or past) the sensor now but was before it last
// step.  A crossing test (rather than a proximity window) guarantees each sensor
// fires exactly once even though the radius advances on a discrete grid.  Sensors
// closer to the source are crossed on earlier sub-steps, so the wave visibly
// propagates across the body over time.
void Life::senseWave(const Circle& waveFront, int64_t timeUs){
    const double radius = waveFront.radius;
    const double prevRadius = radius - WAVE_SPEED; // front position one sub-step ago
    Event e = { timeUs, EventType::wave, -1 };
    for(int k = 0; k < SENSOR_COUNT; ++k){
        const double d = sensorPoint(k).distance(waveFront.center);
        if( prevRadius < d && d <= radius ){ // front crossed this sensor this step
            e.triggeredSensorNumber = k;
            event(e);
        }
    }
}

// A wall wave is a straight line at coordinate loc moving along one axis by
// WAVE_SPEED each sub-step.  A sensor is swept when the line crosses the sensor's
// coordinate this step: the line is at (or past) it now but was on the other side
// last step.  As with circular waves this crossing test fires each sensor exactly
// once as the front passes through the body.
void Life::senseWave(const WallWave& wallWave, int64_t timeUs){
    const double loc = wallWave.getLoc();
    const WallWaveType type = wallWave.getType();
    const bool horizontal = (type == WallWaveType::horisontal_up || type == WallWaveType::horisontal_down);
    // horisontal_down and vertical_right increase loc; the others decrease it.
    const bool increasing = (type == WallWaveType::horisontal_down || type == WallWaveType::vertical_right);
    const double prevLoc = increasing ? loc - WAVE_SPEED : loc + WAVE_SPEED;
    Event e = { timeUs, EventType::wave, -1 };
    for(int k = 0; k < SENSOR_COUNT; ++k){
        const Point2D p = sensorPoint(k);
        const double coord = horizontal ? p.y : p.x;
        const bool crossed = increasing ? (prevLoc < coord && coord <= loc)
                                         : (loc <= coord && coord < prevLoc);
        if( crossed ){
            e.triggeredSensorNumber = k;
            event(e);
        }
    }
}
