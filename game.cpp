// game of life played on the surface of a pond
// where cells have to find each other
// everything that moves ripples
// faster moving objects create more ripples
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
}

void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN){
        getDisplay();
    }
    std::cout << '.';
    std::cout.flush();
}


void Game::draw(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 250, 0, 0, SDL_ALPHA_OPAQUE);
    static int r = 0;
    r = (r+1) % (width/2);
    drawCircle(renderer, width/2, height/2, r);
    SDL_Delay( 1 );
}
