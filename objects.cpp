#include "objects.h"
#include "game.h"   // Game::getInstance()
#include <iostream> // for debugging


void WallWave::draw(SDL_Renderer* rend, int width, int height) const {
    Point2D p1, p2;
    switch(type){
        case WallWaveType::horisontal_down:
        case WallWaveType::horisontal_up:
            p2.x = width;
            p1.y = p2.y = loc;
            break;
        case WallWaveType::vertical_left:
        case WallWaveType::vertical_right:
            p1.x = p2.x = loc;
            p2.y = height;
            break;
    }
    SDL_SetRenderDrawColor(rend, r, g, b, SDL_ALPHA_OPAQUE);
    line(rend, p1, p2);
}


void WallWave::move(){
    switch(type){
        case WallWaveType::horisontal_down: loc+=waveSpeed; break;
        case WallWaveType::horisontal_up:   loc-=waveSpeed; break;
        case WallWaveType::vertical_left:   loc-=waveSpeed; break;
        case WallWaveType::vertical_right:  loc+=waveSpeed; break;
    }
    r = r>fadeSpeed ? r-fadeSpeed : 0.0;
    g = g>fadeSpeed ? g-fadeSpeed : 0.0;
    b = b>fadeSpeed ? b-fadeSpeed : 0.0;
}


// check collision of a WallWave and Life
bool WallWave::checkCollision(const Circle& circle) const {
    switch(type){
        case WallWaveType::horisontal_down:
        case WallWaveType::horisontal_up:
            return (loc <= circle.center.y+circle.radius) && (loc >= circle.center.y-circle.radius);
        case WallWaveType::vertical_left:
        case WallWaveType::vertical_right:
            return (loc <= circle.center.x+circle.radius) && (loc >= circle.center.x-circle.radius);
    }
    return false;
}


// this is the collision angle from the point of view of Life
double WallWave::getCollisionAngle(double angle) const {
    const double NIENTY = 90;
    switch(type){
        case WallWaveType::horisontal_down:
            return NIENTY-angle;
        case WallWaveType::horisontal_up:
            return NIENTY+angle;
        case WallWaveType::vertical_left:
            return -angle;
        case WallWaveType::vertical_right:
            return angle;
    }
    return 0.0;
}


WallWave& WallWave::operator=(const WallWave& other) {
    if (this != &other) {
        r = other.r;
        g = other.g;
        b = other.b;
        loc = other.loc;
        type = other.type;
    }
    return *this;
}


Wave& Wave::operator=(const Wave& other) {
    if (this != &other) {
        r = other.r;
        g = other.g;
        b = other.b;
        circle = other.circle;
    }
    return *this;
}

void Wave::draw(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, r, g, b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

void Wave::move() {
    circle.radius += waveSpeed;
    r = r>fadeSpeed ? r-fadeSpeed : 0.0;
    g = g>fadeSpeed ? g-fadeSpeed : 0.0;
    b = b>fadeSpeed ? b-fadeSpeed : 0.0;
}


void Life::draw(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

void Life::event(const Event& e){
    eventQ.enqueue(e);
// TODO: this is for debugging purposes only till the agent is running
// and able to take items off the queue
// implement sending events to the brain
    Event e2;
    eventQ.dequeue(e2);
}

void Life::action(const Action& a){
    actQ.enqueue(a);
}

void Life::move(const Time& t){
    static Game& game = Game::getInstance();
    static const Point2D unitVector(speed, 0);
    Action a;
    if( actQ.dequeue(a) ){
        switch(a.action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth: break;
            default: return;
        }
        // create a "unit vector", rotate it by angle, translate position using unit vector
        circle.center = circle.center.translate( unitVector.rotate(angle) );
        if( t - lastWave > std::chrono::milliseconds(500) ){
            game.addWave(color, circle.center); // every movement kicks up a wave
            lastWave = t;
        }
    }
}

Life& Life::operator=(const Life& other) {
    if (this != &other) {
        health = other.health;
        color = other.color;
        circle = other.circle;
        velocity = other.velocity;
        angle = other.angle;
        lastWave = other.lastWave;
    }
    return *this;
}
