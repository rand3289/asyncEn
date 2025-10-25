#pragma once
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed

class Game {
    int width;
    int height;
    void getDisplay();
public:
    Game();
    void event(SDL_Event& e);
    void draw(SDL_Renderer* renderer);
};
