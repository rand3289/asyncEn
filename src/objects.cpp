#include "objects.h"
#include "game.h"   // Game::getInstance()
#include <unistd.h> // read()
#include <iostream> // for debugging


void WallWave::draw(SDL_Renderer* rend, int width, int height) const {
    Point2D p1, p2;
    switch(type){
        case WallWaveType::horisontal_down:
        case WallWaveType::horisontal_up:
            p2.x = width;
            p1.y = p2.y = loc;
            break;
        case WallWaveType::vertical_left:
        case WallWaveType::vertical_right:
            p1.x = p2.x = loc;
            p2.y = height;
            break;
    }
    SDL_SetRenderDrawColor(rend, rgb.r, rgb.g, rgb.b, SDL_ALPHA_OPAQUE);
    line(rend, p1, p2);
}


void WallWave::move(){
    switch(type){
        case WallWaveType::horisontal_down: loc+=waveSpeed; break;
        case WallWaveType::horisontal_up:   loc-=waveSpeed; break;
        case WallWaveType::vertical_left:   loc-=waveSpeed; break;
        case WallWaveType::vertical_right:  loc+=waveSpeed; break;
    }
    rgb.r = rgb.r>fadeSpeed ? rgb.r-fadeSpeed : 0.0;
    rgb.g = rgb.g>fadeSpeed ? rgb.g-fadeSpeed : 0.0;
    rgb.b = rgb.b>fadeSpeed ? rgb.b-fadeSpeed : 0.0;
}


// check collision of a WallWave and Life
bool WallWave::checkCollision(const Circle& circle) const {
    switch(type){
        case WallWaveType::horisontal_down:
        case WallWaveType::horisontal_up:
            return (loc <= circle.center.y+circle.radius) && (loc >= circle.center.y-circle.radius);
        case WallWaveType::vertical_left:
        case WallWaveType::vertical_right:
            return (loc <= circle.center.x+circle.radius) && (loc >= circle.center.x-circle.radius);
    }
    return false;
}


// this is the collision angle from the point of view of Life
double WallWave::getCollisionAngle(double angle) const {
    const double NIENTY = 90;
    switch(type){
        case WallWaveType::horisontal_down:
            return NIENTY-angle;
        case WallWaveType::horisontal_up:
            return NIENTY+angle;
        case WallWaveType::vertical_left:
            return -angle;
        case WallWaveType::vertical_right:
            return angle;
    }
    return 0.0;
}


double WallWave::getDistance(const Point2D& p) const {
    if(type==horisontal_down || type == horisontal_up){
        return abs(loc - p.y);
    } else {
        return abs(loc - p.x);
    }
}


WallWave& WallWave::operator=(const WallWave& other) {
    if (this != &other) {
        rgb = other.rgb;
        loc = other.loc;
        type = other.type;
    }
    return *this;
}


Wave& Wave::operator=(const Wave& other) {
    if (this != &other) {
        rgb = other.rgb;
        circle = other.circle;
    }
    return *this;
}

void Wave::draw(SDL_Renderer* rend) const {
    SDL_SetRenderDrawColor(rend, rgb.r, rgb.g, rgb.b, SDL_ALPHA_OPAQUE);
    circle.draw(rend);
}

void Wave::move() {
    circle.radius += waveSpeed;
    rgb.r = rgb.r>fadeSpeed ? rgb.r-fadeSpeed : 0.0;
    rgb.g = rgb.g>fadeSpeed ? rgb.g-fadeSpeed : 0.0;
    rgb.b = rgb.b>fadeSpeed ? rgb.b-fadeSpeed : 0.0;
}


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
            size = sprintf(buff, "1,%d,%d\n", e.time, e.srcAngle);
            break;
        case EventType::collision:
            size = sprintf(buff, "2,%d,%d\n", e.time, e.srcAngle);
            break;
        case EventType::death:
            size = sprintf(buff, "3,%d,0\n", e.time);
            break;
        default: return;
    }
    write(inFd,buff,size);
}

void Life::move(const Time& t){
    static Game& game = Game::getInstance();
    static const Point2D unitVector(speed, 0);

    char buff[128];
    size_t rd = read(outFd, buff, sizeof(buff)-1);
    if( rd > 0 ){
        buff[rd] = 0;
        ActionType action = (ActionType) atoi(buff);
        switch(action){
            case ActionType::kickLeft:  angle-=10.0; break;
            case ActionType::kickRight: angle+=10.0; break;
            case ActionType::kickBoth:  break;
            case ActionType::none:      return;
            default:
                std::cout << buff << std::endl;
                std::cout.flush();
                return;
        }
        // create a "unit vector", rotate it by angle, translate position using unit vector
        circle.center = circle.center.translate( unitVector.rotate(angle) );
        if( t - lastWave > std::chrono::milliseconds(500) ){
            game.addWave(color, circle.center); // every movement kicks up a wave
            lastWave = t;
        }
    }
}

Life& Life::operator=(const Life& other) {
    if (this != &other) {
        health = other.health;
        color = other.color;
        angle = other.angle;
        lastWave = other.lastWave;
        inFd = other.inFd;
        outFd = other.outFd;
        circle = other.circle;
    }
    return *this;
}
