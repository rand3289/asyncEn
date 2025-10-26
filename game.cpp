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

void Game::getDisplay(){
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    width = dm.w;
    height = dm.h;
}

static SDL_Color col;

Game::Game(){
    getDisplay();
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

    for(Life& life: lives){
        life.draw(rend);
        life.move();
        if(life.health <=0){} // TODO: remove from lives
        // need to do collision detection with life and waves
    }

//    for(Wave& wave: waves){
//        wave.draw(rend);
//        wave.move();
//        if( wave.isGone() ){}  // TODO: remove from waves
//    }

    for(int i=0; i< waves.size(); ++i){
        waves[i].draw(rend);
        waves[i].move();
        if( waves[i].isGone() ){ // TODO: remove from waves
            waves.erase(waves.begin() + i);
            --i;
// TEST ONLY:
            waves.emplace_back(col,rand()%width, rand()%height, 10); 
        }
        // need to do collision detection with life
    }

    SDL_Delay( 10 );
}


// for collision detection: sort all life by how far they are from the center of the screen.
// determine collisions between life within a sliding window
