// The game is played on the surface of a pond so everything that moves ripples
#include "game.h"
#include "agents.h"   // spawnAgents()
#include "geometry.h"
#include <SDL2/SDL.h> // Simple Directmedia Layer lib has to be installed
#include <iostream>   // for debugging
#include <thread>     // sleep_for()
#include <algorithm>  // sort()
#include <chrono>
using namespace std::chrono;


// extrapolate how a straight wave (line) travels through a circle of radius 'radius'
// from left to right on the interval of time from (time - frameTime) till time 'time'
//
// 'angle' is the angle at which the wave approaches/enters the circle
// 'distance' is the distance from the center of the circle to the line of the wave
void extrapolateWaveTravel(Life& life, double angle, double distance, const Time& time, const milliseconds& frameTime) {
    const int SENSOR_COUNT = 8; // 3+
    static Event evt = { .event = EventType::wave, .srcAngle = angle };
    // AI TODO: 
    // Life is represented by a circle with a radius 'life.circle.radius'
    // Imagine 'SENSOR_COUNT' number of sensors at equal distance apart around the circumference.
    // Implement this function to call life.event(evt) when a wave moving from left to right 
    // with 'waveSpeed' hits one of the sensors.
}

void extrapolateWaveTravel(Life& life, const Wave& wave, const Time& time, const milliseconds& frameTime){
    double angle = life.circle.center.angle(wave.circle.center) + life.getAngle();
    double distance = life.circle.center.distance(wave.circle.center) - wave.circle.radius;
    extrapolateWaveTravel(life, angle, distance, time, frameTime);
}

void extrapolateWaveTravel(Life& life, const WallWave& wave, const Time& time, const milliseconds& frameTime){
    double angle = wave.getCollisionAngle(life.getAngle());
    double distance = wave.getDistance(life.circle.center);
    extrapolateWaveTravel(life, angle, distance, time, frameTime);
}


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


void Game::addWave(const SDL_Color& color, const Point2D& p){
    waves.emplace_back(color, p.x, p.y, 1); 
}


// reduce the number of waves in the system to level framerate
void Game::cleanupWaves(std::chrono::high_resolution_clock::time_point& time){
    // prevent cleanup two frames in a row
    // static auto lastTime = std::chrono::high_resolution_clock::now();
    // if(time-lastTime < std::chrono::milliseconds(128)){ return; }

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


void Game::draw(SDL_Renderer* rend, int width, int height){
    const int fps = 30;
    const auto frameTime = std::chrono::milliseconds(1000/fps);
    static auto nextTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::high_resolution_clock::now();

    if(time < nextTime){ // make sure framerate (FPS) is steady
        std::this_thread::sleep_for(nextTime-time);
    } else {
        cleanupWaves(time);
    }
    nextTime += frameTime;

    // check collisions with walls and push circles away from the walls
    Event e = {time, EventType::collision, 0.0};
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
        life.event(e);
    }

    // check collisions among lives
    for (size_t i = 0; i < lives.size(); ++i) {
        for (size_t j = i + 1; j < lives.size(); ++j) {
            if ( lives[i].circle.checkCollision(lives[j].circle) ) {
                e.srcAngle = lives[i].circle.center.angle(lives[j].circle.center);
                lives[i].event(e);
                e.srcAngle += 180;
                lives[j].event(e);
                lives[i].circle.pushApart(lives[j].circle); // push collided lives apart
                // TODO: shouldn't collisions cause waves ???
            }
        }
    }

    // check collisions between lives and waves
    e.event = EventType::wave;
    for (Life& life: lives) {
        for (const Wave& wave: waves) {
            if( life.circle.checkCollision(wave.circle) && !wave.circle.inside(life.circle) ){ // once wave passes Life, stop sending events
                e.srcAngle = life.circle.center.angle(wave.circle.center);
                life.event(e); // waves don't get events
//                extrapolateWaveTravel(life, wave, time);
//extrapolateWaveTravel(life.circle.center.x, life.circle.center.y, life.getAngle(), life.circle.radius, time, frameTime, wave.circle.center.x, wave.circle.center.y, wave.circle.radius);
// TODO: extrapolate a wave movement through a Life (send multiple events) see wave.html for more info
            }
        }
    }

    // check collisions between lives and wall waves (waves generated by collisions with walls)
    for (Life& life: lives) {
        for (const WallWave& wave: wallWaves) {
            if(wave.checkCollision(life.circle)){
                e.srcAngle = wave.getCollisionAngle(life.getAngle());
                life.event(e);
// TODO: extrapolate a wave movement through a Life (send multiple events) see wave.html for more info
            }
        }
    }

    // draw and move lives or delete them if health < 0
    e.event = EventType::death;
    for(int i=0; i < lives.size(); ++i){
        if(lives[i].getHealth() <= 0){
            lives[i].event(e);
            lives.erase(lives.begin() + i);
            --i;
            continue;
        }
        lives[i].draw(rend);
        lives[i].move(time);
    }

    // draw and move waves or delete them if health < 0
    for(int i=0; i< waves.size(); ++i){
        if(waves[i].getHealth() <= 0){ // remove waves that have dissipated
            waves.erase(waves.begin() + i);
            --i;
            continue;
        }
        waves[i].draw(rend);
        waves[i].move();
    }

    // draw and move waves from the walls or delete them if health < 0
    for(int i=0; i< wallWaves.size(); ++i){
        if(wallWaves[i].getHealth() <= 0){ // remove waves that have dissipated
            wallWaves.erase(wallWaves.begin() + i);
            --i;
            continue;
        }
        wallWaves[i].draw(rend, width, height);
        wallWaves[i].move();
    }
} // Game::draw()
