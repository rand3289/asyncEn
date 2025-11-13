#include "objs.h"
#include "game.h"
#include <iostream>


void WallWave::draw(SDL_Renderer* rend, int width, int height){
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

void Wave::draw(SDL_Renderer* rend) {
    SDL_SetRenderDrawColor(rend, r, g, b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

void Wave::move() {
    circle.radius += waveSpeed;
    r = r>fadeSpeed ? r-fadeSpeed : 0.0;
    g = g>fadeSpeed ? g-fadeSpeed : 0.0;
    b = b>fadeSpeed ? b-fadeSpeed : 0.0;
}


void Life::draw(SDL_Renderer* rend){
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

void Life::event(const Event& e){
    switch(e.event){
        case EventType::collision: std::cout << "x"; break;
        case EventType::wave: std::cout << "_"; break;
    }
    std::cout.flush();
}

void Life::action(const Action& a){
    actQ.enqueue(a);
}

void Life::move(){
    Action a;
    if( actQ.dequeue(a) ){
        switch(a.action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth: break;
            default: return;
        }
        circle.center = circle.center.translate( Point2D(5.0,0).rotate(angle) );
        Game& game = Game::getInstance(); // TODO: this could be replaced by returning true/false and performing this operation in Game::draw()
        game.addWave(color, circle.center);
    }
}

Life& Life::operator=(const Life& other) {
    if (this != &other) {
        health = other.health;
        color = other.color;
        circle = other.circle;
        velocity = other.velocity;
        angle = other.angle;
    }
    return *this;
}
