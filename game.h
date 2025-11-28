#pragma once
#include "objects.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <vector>


class Game {
    std::vector<WallWave> wallWaves;
    std::vector<Wave> waves;
    std::vector<Life> lives;
    std::vector<int> inFds;  // file descriptors
    std::vector<int> outFds; // file descriptors
    void cleanupWaves(std::chrono::high_resolution_clock::time_point& time);
    void log();
public:
    static Game& getInstance();
    void event(SDL_Event& e);
    void draw(SDL_Renderer* rend, int width, int height);
    void addWave(const SDL_Color& color, const Point2D& p);
};
