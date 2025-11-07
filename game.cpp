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

void Game::getDisplay(){
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    width = dm.w;
    height = dm.h;
}

static SDL_Color col;

Game::Game(){
    getDisplay();

// TESTING:
    col.a = SDL_ALPHA_OPAQUE;
    col.r = 255;
    col.g = 0;
    col.b = 0;
    waves.emplace_back(col, 500,500,10);
    waves.emplace_back(col, 100,100,10);
}

void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN){
        getDisplay();
    }
    std::cout << '.';
    std::cout.flush();
}


void Game::draw(SDL_Renderer* rend){
    const int fps = 60;
    const auto frameTime = std::chrono::milliseconds(1000/fps);

    static std::chrono::high_resolution_clock::time_point nextTime;
    auto time = std::chrono::high_resolution_clock::now();

    if(time < nextTime){
        std::this_thread::sleep_for(nextTime-time);
    }
    nextTime += frameTime;

// need to do collision detection with other lives and waves
// for collision detection: sort all life by how far they are from the center of the screen.
// determine collisions between life within a sliding window
    for(Life& life: lives){
        life.draw(rend);
        life.move();
        if(life.health <=0){} // TODO: remove from lives
    }

    for(int i=0; i< waves.size(); ++i){
        waves[i].draw(rend);
        waves[i].move();
        if( waves[i].isGone() ){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
// TEST ONLY:
            waves.emplace_back(col, rand() % (int)width, rand() % (int)height, 10.0);
        }
    }

    SDL_Delay( 10 );
}
