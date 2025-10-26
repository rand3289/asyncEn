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

Game::Game(){
    getDisplay();
    SDL_Color col;
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
        if(life.health <=0){} // TODO: remove from lives
    }

    for(Wave& wave: waves){
        wave.draw(rend);
        wave.move();
        if( wave.isGone() ){}  // TODO: remove from waves
    }

//    SDL_SetRenderDrawColor(rend, 250, 250, 250, SDL_ALPHA_OPAQUE);
//    static int r = 0;
//    r = (r+1) % (width/2);
//    drawCircle(rend, width/2, height/2, r);

    SDL_Delay( 10 );
}


// for collision detection: sort all life by how far they are from the center of the screen.
// determine collisions between life within a sliding window
