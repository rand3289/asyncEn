// The game is played on the surface of a pond so everything that moves ripples
#include "game.h"
#include "agents.h"   // spawnAgents()
#include "geometry.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>   // for debugging
#include <thread>     // sleep_for()
#include <algorithm>  // sort()
#include <chrono>
#include <cmath>      // for M_PI, cos, sin, exp


Game& Game::getInstance() {
    static Game* instance;
    if(!instance){
        instance = new Game();

        std::vector<int> inFds;  // file descriptors
        std::vector<int> outFds; // file descriptors
        spawnAgents(inFds, outFds);

        for(size_t i = 0; i < inFds.size(); ++i){
            instance->lives.emplace_back(inFds[i], outFds[i]);
        }
    }
    return *instance;
}


void Game::addWave(const RGB& color, const Point2D& p){
    waves.emplace_back(color, p.x, p.y, 1); 
}


// reduce the number of waves in the system to level framerate
void Game::cleanupWaves(){
    std::sort(wallWaves.begin(), wallWaves.end()); // by type and location

    for(auto it = wallWaves.begin(); it != wallWaves.end() && (it+1) != wallWaves.end(); ++it){
        if( it->equal(*(it+1)) ){
            wallWaves.erase(it+1); // TODO: this is slow
        }
    }

    const size_t ws = waves.size();
    for(int i=1; i < ws/2; i+=2){
        waves[i] = waves[ws-i]; // mix new waves from the back into the old waves in front
    }
    waves.resize(ws/2); // erase 50% of waves

    std::cout << '.';
    std::cout.flush();
}


void Game::event(SDL_Event& e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_SPACE){
            std::cout << std::endl << "L=" << lives.size() << ", W=" << waves.size() << ", WW="<< wallWaves.size() << std::endl;
            std::cout.flush();
        }
    }
}


int64_t timeNowUs(){ // current time in microseconds
    const static auto epoch = std::chrono::floor<std::chrono::days>(std::chrono::high_resolution_clock::now());
    auto now = std::chrono::high_resolution_clock::now();
    return (now - epoch).count() / 1000; // microseconds from the beginning of the day
}


void Game::draw(SDL_Renderer* rend, int width, int height){
    constexpr int64_t usps = 1000000;       // microseconds per second
    constexpr int64_t fps = 30;             // frames per second
    constexpr int64_t frameTime = usps/fps; // one video frame in microseconds
    constexpr int64_t movesPerFrame = 10;   // state updates per video frame
    constexpr int64_t moveTime = (usps / fps) / movesPerFrame;

    static int64_t nextTime = timeNowUs(); // initialized once
    int64_t time = timeNowUs();

    if(time < nextTime){ // make sure framerate (FPS) is steady
        std::this_thread::sleep_for(std::chrono::microseconds(nextTime-time));
    } else { // Running takes too long.  Reduce the number of waves.
        cleanupWaves();
    }
    nextTime += frameTime;

    for(int i=0; i < lives.size(); ++i){
        lives[i].draw(rend);
    }
    for(int i=0; i< waves.size(); ++i){
        waves[i].draw(rend);
    }
    for(int i=0; i< wallWaves.size(); ++i){
        wallWaves[i].draw(rend, width, height);
    }

    // We do not draw every update.  This allows for more granular updates.
    // If we run simulation at several re-computes per frame,
    // we can hit sensors on "lives" with different timestamps as wave propagetes through it.
    // This is essential to our simulation!
    for(int i=0; i < movesPerFrame; ++i){
        move(width, height, time);
        time += moveTime;
    }
}


void Game::move(int width, int height, int64_t timeUs){
    // check collisions with walls and push circles away from the walls
    Event e = {timeUs, EventType::collision, 0.0};
    for(Life& life: lives){
        if(life.circle.center.x <= 0){
            life.circle.center.x = life.circle.radius;
            e.srcAngle = 180-life.getAngle();
            wallWaves.emplace_back(0, WallWaveType::vertical_right);
        } else if (life.circle.center.x >= width){
            life.circle.center.x = width-life.circle.radius;
            e.srcAngle = -life.getAngle();
            wallWaves.emplace_back(width, WallWaveType::vertical_left);
        } else if (life.circle.center.y <= 0){
            life.circle.center.y = life.circle.radius;
            e.srcAngle = 90-life.getAngle();
            wallWaves.emplace_back(0, WallWaveType::horisontal_down);
        } else if(life.circle.center.y >= height){
            life.circle.center.y = height-life.circle.radius;
            e.srcAngle = -90-life.getAngle();
            wallWaves.emplace_back(height, WallWaveType::horisontal_up);
        } else { continue; }
        e.srcAngle = normalizeAngle(e.srcAngle);
        life.event(e);
    }

    // check collisions among lives
    for (size_t i = 0; i < lives.size(); ++i) {
        for (size_t j = i + 1; j < lives.size(); ++j) {
            if ( lives[i].circle.checkCollision(lives[j].circle) ) {
                e.srcAngle = lives[i].circle.center.angle(lives[j].circle.center);
                e.srcAngle = normalizeAngle(e.srcAngle);
                lives[i].event(e);
                e.srcAngle += 180;
                e.srcAngle = normalizeAngle(e.srcAngle);
                lives[j].event(e);
                lives[i].circle.pushApart(lives[j].circle); // push collided lives apart
                // TODO: shouldn't collisions cause waves ???
            }
        }
    }

    // move lives or delete them if health < 0
    e.event = EventType::death;
    for(int i=0; i < lives.size(); ++i){
        if(lives[i].getHealth() <= 0){
            lives[i].event(e);
            lives.erase(lives.begin() + i);
            --i;
            continue;
        }
        lives[i].move(timeUs);
    }

    // move waves or delete them if health < 0
    for(int i=0; i< waves.size(); ++i){
        if(waves[i].getHealth() <= 0){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
            continue;
        }
        waves[i].move();
    }

    // move waves produced by walls or delete them if health < 0
    for(int i=0; i< wallWaves.size(); ++i){
        if(wallWaves[i].getHealth() <= 0){ // remove waves that have dissipated
            wallWaves.erase(wallWaves.begin() + i);
            --i;
            continue;
        }
        wallWaves[i].move();
    }

    // check collisions between lives and waves
    e.event = EventType::wave;
    for (Life& life: lives) {
        for (const Wave& wave: waves) {
            if( life.circle.checkCollision(wave.circle) && !wave.circle.inside(life.circle) ){ // once wave passes Life, stop sending events
                e.srcAngle = life.circle.center.angle(wave.circle.center);
                life.event(e);
            }
        }
    }

    // check collisions between lives and wall waves (waves generated by collisions with walls)
    for (Life& life: lives) {
        for (const WallWave& wave: wallWaves) {
            if(wave.checkCollision(life.circle)){
                e.srcAngle = wave.getCollisionAngle(life.getAngle());
                life.event(e);
            }
        }
    }
} // Game::draw()
