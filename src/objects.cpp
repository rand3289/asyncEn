#include "objects.h"
#include "game.h"   // Game::getInstance()
#include <unistd.h> // read()
#include <iostream> // for debugging


void Life::draw(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

// writes Event into inFd
void Life::event(const Event& e){
    int size;
    char buff[32];
    switch(e.event){
        case EventType::wave:
            size = sprintf(buff, "1,%lld,%d\n", e.time, (int) e.srcAngle);
            break;
        case EventType::collision:
            size = sprintf(buff, "2,%lld,%d\n", e.time, (int) e.srcAngle);
            break;
        case EventType::death:
            size = sprintf(buff, "3,%lld,0\n", e.time);
            break;
        default: return;
    }
    write(inFd,buff,size);
}

void Life::move(int64_t timeUs){
    static const Point2D unitVector(speed, 0);

    char buff[128];
    size_t rd = read(outFd, buff, sizeof(buff)-1);
    if( rd > 0 ){
        buff[rd] = 0;
        int time, action, param;
        if(3 != sscanf(buff, "%d,%d,%d\n", &time, &action, &param)){
            return;
        }
        switch(action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth:  break;
            case ActionType::none:      return;
            default:
                std::cout << "\nUnknown command from agent: " << buff << std::endl;
                std::cout.flush();
                return;
        }
        // create a "unit vector", rotate it by angle, translate position using unit vector
        circle.center = circle.center.translate( unitVector.rotate(angle) );

        if( timeUs > nextWaveTimeUs ){
            static Game& game = Game::getInstance();
            game.addWave(color, circle.center);    // every movement kicks up a wave
            constexpr int64_t halfSecond = 500000; // in microseconds
            nextWaveTimeUs = timeUs+halfSecond;
        }
    }
}

Life& Life::operator=(const Life& other) {
    if (this != &other) {
        health = other.health;
        color = other.color;
        angle = other.angle;
        nextWaveTimeUs = other.nextWaveTimeUs;
        inFd = other.inFd;
        outFd = other.outFd;
        circle = other.circle;
    }
    return *this;
}
