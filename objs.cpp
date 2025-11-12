#include "objs.h"
#include "game.h"


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

void Life::event(const Event& e){}

void Life::action(const Action& a){
    actQ.enqueue(a);
}

void Life::move(){
    Action a;
    if( actQ.dequeue(a) ){
        Game& game = Game::getInstance();
        game.addWave(color, circle.center);
        switch(a.action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth: break;
            default: return;
        }
        circle.center = circle.center.translate( Point2D(5.0,0).rotate(angle) );
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
