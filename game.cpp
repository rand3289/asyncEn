// game of life played on the surface of a pond
// everything that moves ripples
// entities look like tubers in a lazy river

#include "game.h"
#include "misc.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>
#include <string>
#include <chrono>
#include <thread> // sleep_for()


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
}

void Game::addWave(const SDL_Color& color, const Point2D& p){
    waves.emplace_back(color, p.x, p.y, 1); 
}


void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYDOWN){
        Action a = {.action = ActionType::none };
        switch(e.key.keysym.sym){
            case SDLK_RETURN: getDisplay(); break;
            case SDLK_LEFT:   a.action = ActionType::kickLeft;  break;
            case SDLK_RIGHT:  a.action = ActionType::kickRight; break;
            case SDLK_UP:     a.action = ActionType::kickBoth;  break;
            case SDLK_DOWN:   lives.emplace_back(); break;
        }
        for(Life& life: lives){
            life.action(a);
        }
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

    // check collisions among lives
    Event e = {EventType::collision, time, 0.0};
    for (size_t i = 0; i < lives.size(); ++i) {
        for (size_t j = i + 1; j < lives.size(); ++j) {
            if ( lives[i].circle.checkCollision(lives[j].circle) ) {
                double angle = lives[i].circle.center.angle(lives[j].circle.center);
                e.srcAngle = angle;
                lives[i].event(e);
                e.srcAngle = angle+180;
                lives[j].event(e);
            }
        }
    }

    // check collisions between lives and waves
    e.event = EventType::wave;
    for (Life& life: lives) {
        for (const Wave& wave: waves) {
            if( life.circle.checkCollision(wave.circle) && !wave.circle.inside(life.circle) ){
                life.event(e);
            }
        }
    }

    for(int i=0; i < lives.size(); ++i){
        if(lives[i].getHealth() <= 0){
            e.event = EventType::death;
            lives[i].event(e);
            lives.erase(lives.begin() + i);
            --i;
            continue;
        }
        lives[i].draw(rend);
        lives[i].move();
    }

    for(int i=0; i< waves.size(); ++i){
        if(waves[i].getHealth() <= 0){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
            continue;
        }
        waves[i].draw(rend);
        waves[i].move();
    }
}
