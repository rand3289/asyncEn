// A human controlled agent for asynEn environment
// It creates a blank window, captures LEFT/UP/RIGHT keys and prints corresponding 1/2/3 to stdout
// Agents implemented as a console applications do not show a window
// g++ keypad.cpp -o keypad -lSDL2
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>
#include <string>
#include <thread> // sleep_for()
#include <chrono>
using namespace std::chrono;


void exitSDLerr(){
    std::cerr << "SDL error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
}


int main(int argc, char* argv[]){
    const int WIDTH = 200;
    const int HEIGHT = 200;
    const int WINPOS = SDL_WINDOWPOS_CENTERED;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) { exitSDLerr(); }
    SDL_Window* window=SDL_CreateWindow("keypad", WINPOS, WINPOS, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if(0==window){ exitSDLerr(); }
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    if(0==renderer){ exitSDLerr(); }
//        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);

    SDL_Event e;
    bool run = true;
    while(run){
        while( SDL_PollEvent( &e ) ){
            if(e.type == SDL_QUIT){ run=false; }
            else if (e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE: run=false;       break;
                    case SDLK_LEFT:  std::cout << '1'; break;
                    case SDLK_UP:    std::cout << '3'; break;
                    case SDLK_RIGHT: std::cout << '2'; break;
                    default:                           break;
                }
                std::cout.flush();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(milliseconds(10));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
