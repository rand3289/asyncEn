// game of life played on the surface of a pond
// everything that moves ripples
// entities look like tubers in a lazy river

#include "game.h"
#include "misc.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream> // for debugging
#include <chrono>
#include <thread> // sleep_for()


Game& Game::getInstance() {
    static Game instance;
    return instance;
}


void Game::addWave(const SDL_Color& color, const Point2D& p){
    waves.emplace_back(color, p.x, p.y, 1); 
}


void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYDOWN){
        Action a = {.action = ActionType::none };
        switch(e.key.keysym.sym){
            case SDLK_LEFT:   a.action = ActionType::kickLeft;  break;
            case SDLK_RIGHT:  a.action = ActionType::kickRight; break;
            case SDLK_UP:     a.action = ActionType::kickBoth;  break;
            case SDLK_DOWN:   lives.emplace_back(); return;
            default: return;
        }
        for(Life& life: lives){
            life.action(a);
        }
    }
}


void Game::draw(SDL_Renderer* rend, int width, int height){
    const int fps = 60;
    const auto frameTime = std::chrono::milliseconds(1000/fps);
    static auto nextTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::high_resolution_clock::now();

    if(time < nextTime){ // this makes sure framerate (FPS) is steady
        std::this_thread::sleep_for(nextTime-time);
    }
    nextTime += frameTime;

    Event e = {EventType::collision, time, 0.0};
    // check collisions with walls
    for(Life& life: lives){
        if(life.circle.center.x <= 0){
            life.circle.center.x = life.circle.radius;
            e.srcAngle = 180-life.getAngle();
            wallWaves.emplace_back(0, WallWaveType::vertical_right);
        } else if (life.circle.center.x >= width){
            life.circle.center.x = width-life.circle.radius;
            e.srcAngle = -life.getAngle();
            wallWaves.emplace_back(width, WallWaveType::vertical_left);
        } else if (life.circle.center.y <= 0){
            life.circle.center.y = life.circle.radius;
            e.srcAngle = 90-life.getAngle();
            wallWaves.emplace_back(0, WallWaveType::horisontal_down);
        } else if(life.circle.center.y >= height){
            life.circle.center.y = height-life.circle.radius;
            e.srcAngle = -90-life.getAngle();
            wallWaves.emplace_back(height, WallWaveType::horisontal_up);
        } else { continue; }
        life.event(e);
    }

    // TODO: set lives back from each other when they collide
    // check collisions among lives
    for (size_t i = 0; i < lives.size(); ++i) {
        for (size_t j = i + 1; j < lives.size(); ++j) {
            if ( lives[i].circle.checkCollision(lives[j].circle) ) {
                e.srcAngle = lives[i].circle.center.angle(lives[j].circle.center);
                lives[i].event(e);
                e.srcAngle += 180;
                lives[j].event(e);
            }
        }
    }

    // check collisions between lives and waves
    e.event = EventType::wave;
    for (Life& life: lives) {
        for (const Wave& wave: waves) {
            if( life.circle.checkCollision(wave.circle) && !wave.circle.inside(life.circle) ){
                e.srcAngle = life.circle.center.angle(wave.circle.center);
                life.event(e); // waves don't get events
            }
        }
    }

    for (Life& life: lives) {
        for (const WallWave& wave: wallWaves) {
// TODO: implement wallWave collisions
//            if( life.circle.checkCollision(wave.circle) ){
//                double angle = life.circle.center.angle(wave.circle.center);
//                e.srcAngle = angle;
//                life.event(e);
//            }
        }
    }

    e.event = EventType::death;
    // draw and move lives or delete them if health < 0
    for(int i=0; i < lives.size(); ++i){
        if(lives[i].getHealth() <= 0){
            lives[i].event(e);
            lives.erase(lives.begin() + i);
            --i;
            continue;
        }
        lives[i].draw(rend);
        lives[i].move();
    }

    // draw and move waves or delete them if health < 0
    for(int i=0; i< waves.size(); ++i){
        if(waves[i].getHealth() <= 0){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
            continue;
        }
        waves[i].draw(rend);
        waves[i].move();
    }

    // draw and move waves from the walls or delete them if health < 0
    for(int i=0; i< wallWaves.size(); ++i){
        if(wallWaves[i].getHealth() <= 0){ // remove waves that have dissipated
            wallWaves.erase(wallWaves.begin() + i);
            --i;
            continue;
        }
        wallWaves[i].draw(rend, width, height);
        wallWaves[i].move();
    }
} // Game::draw()
