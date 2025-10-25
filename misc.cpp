#include "misc.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed

void line(SDL_Renderer* rend, const Point2D& p1, const Point2D& p2){
    SDL_RenderDrawLine(rend, p1.x, p1.y , p2.x, p2.y);
}

// https://discourse.libsdl.org/t/query-how-do-you-draw-a-circle-in-sdl2-sdl2/33379
void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius){
    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y){
        // Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0){
  	        ++y;
  	        error += ty;
  	        ty += 2;
        }
        if (error > 0){
  	        --x;
  	        tx += 2;
  	        error += (tx - diameter);
        }
    }
}


