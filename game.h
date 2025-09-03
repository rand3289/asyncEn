#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed

struct Game {
    void event(SDL_Event& e);
    void draw(SDL_Renderer* renderer);
};
