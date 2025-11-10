#include "objs.h"
#include "game.h"

void Life::draw(SDL_Renderer* rend){}
void Life::event(const Event& e){}

void Life::action(const Action& a){
    actQ.enqueue(a);
}

void Life::move(){
    Action a;
    if( actQ.dequeue(a) ){
        Game& game = Game::getInstance();
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
