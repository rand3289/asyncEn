#include "misc.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed


void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius){
    int32_t x = radius;
    int32_t y = 0;
    int32_t err = 0;

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

        if (err <= 0){
  	        ++y;
  	        err += y;
        } else {
  	        --x;
  	        err -= x;
        }
    }
}
