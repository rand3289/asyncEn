#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>
#include <string>
#include "game.h"


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


void Game::event(SDL_Event& e){
    if(e.type == SDL_USEREVENT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN) ){
        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        width = dm.w;
        height = dm.h;
    }
    std::cout << '.';
    std::cout.flush();
}

void runTests(SDL_Renderer* rend, int x, int y);

void Game::draw(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 250, 0, 0, SDL_ALPHA_OPAQUE);
    static int r = 0;
    r = (r+1) % (width/2);
    drawCircle(renderer, width/2, height/2, r);
    runTests(renderer, width/2, height/2);
    SDL_Delay( 1 );
}


struct Point {
    double x,y,z;
    Point(): x(0.0), y(0.0), z(0.0) {}
    Point(double X, double Y, double Z): x(X), y(Y), z(Z) {}
    Point translate(const Point& p) const { return Point(x+p.x, y+p.y, z+p.z); }
//    double  dot(const Point& p) const { return x*p.x + y*p.y + z*p.z; }
//    Point cross(const Point& p) const { return Point( y*p.z-z*p.y,  z*p.x-x*p.z,  x*p.y-y*p.x ); }
    // https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/3drota.htm
    Point rotateX(const double a) const { // rotate around X axis
        const double sa = sin(a);
        const double ca = cos(a);
        return Point( x, y*ca-z*sa, y*sa+z*ca );
    }
    Point rotateY(const double a) const { // rotate around Y axis
        const double sa = sin(a);
        const double ca = cos(a);
        return Point( z*sa+x*ca, y, z*ca-x*sa );
    }
    Point rotateZ(const double a) const { // rotate around z axis
        const double sa = sin(a);
        const double ca = cos(a);
        return Point( x*ca-y*sa, y*ca+x*sa, z );
    }
    // https://math.stackexchange.com/questions/1741282/3d-calculate-new-location-of-point-after-rotation-around-origin
    Point rotate(const double a, const double b) const { // no twist
        const double sa = sin(a);
        const double ca = cos(a);
        const double sb = sin(b);
        const double cb = cos(b);
        return Point( x*(ca*cb)-y*(sa*sb)+x*(sb), y*(ca*sb+sa*sb)+y*(ca*cb-sa*sb)-y*(sa*cb), z*(sa-ca*sb)+z*(sa+ca*sb)+z*(ca*cb) );
    }
};


void line(SDL_Renderer* rend, const Point& p1, const Point& p2){
    SDL_RenderDrawLine(rend, p1.x, p1.y , p2.x, p2.y);
}


void runTests(SDL_Renderer* rend, int x, int y){
    static double angle = 0.0;
    angle+=0.001;
    SDL_SetRenderDrawColor(rend, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE); // green

    Point v0(x,y,0);
    Point v1(300,0,0);
    Point v2(0,300,0);
    Point v3 = v2.rotateX(angle).translate(v0);
    Point v4 = v1.rotateY(angle).translate(v0);
    Point v5 = v1.rotateZ(angle).translate(v0);
    line(rend, v0, v3);
    line(rend, v0, v4);
    line(rend, v0, v5);
}
