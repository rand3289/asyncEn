#pragma once
#include "objects.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <vector>


class Game {
    std::vector<WallWave> wallWaves;
    std::vector<Wave> waves;
    std::vector<Life> lives;
    void cleanupWaves(std::chrono::high_resolution_clock::time_point& time);
public:
    static Game& getInstance();
    void event(SDL_Event& e);
    void draw(SDL_Renderer* rend, int width, int height);
    void addWave(const RGB& color, const Point2D& p);
};
