#pragma once
#include "objs.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <vector>


class Game {
    double width;
    double height;
    std::vector<Wave> waves;
    std::vector<Life> lives;

    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    void getDisplay();
public:
    static Game& getInstance();
    void event(SDL_Event& e);
    void draw(SDL_Renderer* rend);
    void addWave(const SDL_Color& color, const Point2D& p);
};
