#pragma once
#include "objects.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <vector>


class Game {
    std::vector<WallWave> wallWaves;
    std::vector<Wave> waves;
    std::vector<Life> lives;
    void cleanupWaves();
public:
    static Game& getInstance();
    void event(SDL_Event& e);
    void addWave(const RGB& color, const Point2D& p);
    void draw(SDL_Renderer* rend, int width, int height);
    void move(int width, int height, int64_t timeUs);
};
