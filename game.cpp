// game of life played on the surface of a pond
// where cells have to find each other
// everything that moves ripples
// faster moving objects create more ripples
// entities look like tubers in a lazy river
// 3 entities that eat each other

#include "game.h"
#include "misc.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>
#include <string>
#include <chrono>
#include <thread> // sleep_for()

static SDL_Color col = {255,0,0,SDL_ALPHA_OPAQUE};

bool checkCollision(const Circle& a, const Circle& b) {
    return a.distance(b) <= (a.radius + b.radius);
}

double calcAngle(Point2D& p1, Point2D& p2){
    return 0;
}

/**********************************************************************/
Game& Game::getInstance() {
    static Game instance;
    return instance;
}

void Game::getDisplay(){
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    width = dm.w;
    height = dm.h;
}

Game::Game(){
    getDisplay();
    lives.emplace_back();
}

void Game::addWave(const SDL_Color& color, const Point2D& p){
    waves.emplace_back(color, p.x, p.y, 1);
}


void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN){
        getDisplay();
    }
    std::cout << '.';
    std::cout.flush();

// TODO: testing only:
    Action a;
    a.action = ActionType::kickLeft;
    for(Life& life: lives){
        life.action(a);
    }
}

void Game::draw(SDL_Renderer* rend){
    const int fps = 60;
    const auto frameTime = std::chrono::milliseconds(1000/fps);

    static std::chrono::high_resolution_clock::time_point nextTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::high_resolution_clock::now();

    if(time < nextTime){
        std::this_thread::sleep_for(nextTime-time);
    }
    nextTime += frameTime;

    Event e = {time, EventType::collision, 0.0};

    for (size_t i = 0; i < lives.size(); ++i) {
        for (size_t j = i + 1; j < lives.size(); ++j) {
            if ( checkCollision(lives[i].circle, lives[j].circle) ) {
                double angle = calcAngle(lives[i].circle.center, lives[j].circle.center);
                e.srcAngle = angle;
                lives[i].event(e);
                e.srcAngle = angle+180;
                lives[j].event(e);
            }
        }
    }

    e.event = EventType::wave;
    for (Life& life: lives) {
        for (const Wave& wave: waves) {
            if( checkCollision(life.circle, wave.circle) ){
                life.event(e);
            }
        }
    }

    for(int i=0; i < lives.size(); ++i){
        if(lives[i].health <=0){
            lives.erase(lives.begin() + i);
            --i;
            continue;
        }
        lives[i].draw(rend);
        lives[i].move();
    }

    for(int i=0; i< waves.size(); ++i){
        if( waves[i].isGone() ){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
            continue;
        }
        waves[i].draw(rend);
        waves[i].move();
    }
}
