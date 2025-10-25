// asyncEn is an asynchronous environment based on SDL
#include "game.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>
#include <string>

void toggleFS(SDL_Window* win){
    bool fs = SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(win, fs ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void exitSDLerr(){
    std::cerr << "SDL error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
}

int main(int argc, char* argv[]){
    const int WIDTH = 1024;
    const int HEIGHT = 768;
    const int WINPOS = SDL_WINDOWPOS_CENTERED;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) { exitSDLerr(); }

    SDL_Window* window=SDL_CreateWindow("asyncEn", WINPOS, WINPOS, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if(0==window){ exitSDLerr(); }
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    if(0==renderer){ exitSDLerr(); }
//    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); // SDL_WINDOW_FULLSCREEN for different resolution

    Game game;
    SDL_Event e;
    bool run = true;
    while(run){
        while( SDL_PollEvent( &e ) ){
            if(e.type == SDL_QUIT){ run=false; }
            else if(e.type == SDL_KEYUP ){
                switch(e.key.keysym.sym){
                    case SDLK_RETURN: toggleFS(window); break;
                    case SDLK_ESCAPE: run=false;        break;
                    default:                            break;
                }
            }
            game.event(e);
        }

        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);

        game.draw(renderer);
        SDL_RenderPresent(renderer);
//        SDL_Delay( 16 ); // less than 60fps
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
