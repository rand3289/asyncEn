#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed

struct Game {
    int width;
    int height;
    Game(): width(0), height(0) {}
    void event(SDL_Event& e);
    void draw(SDL_Renderer* renderer);
};
