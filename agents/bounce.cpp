// A straight-line "bouncing ball" agent for the asyncEn environment.
//
// Behaviour:
//   * Moves in a straight line at full speed by continuously issuing the
//     "kickBoth" action (3), which advances the body without turning.
//   * When it receives a collision event (EventType::collision -> wire type 2)
//     it picks a new random heading and turns onto it, so it ricochets off the
//     wall at a random angle instead of a mirror-reflection angle.
//
// Protocol (see src/life.cpp):
//   Incoming events on stdin : "type,time,sensor\n"
//       type 1 = wave, 2 = collision, 3 = death
//   Outgoing actions on stdout: "time,action,param\n"
//       action 0 = none, 1 = kickLeft (-10 deg), 2 = kickRight (+10 deg),
//       action 3 = kickBoth (move straight ahead)
//
// The engine only rotates the heading by +/-10 degrees per kick, so a bounce to
// an arbitrary angle is performed as a sequence of single-step kicks. While a
// turn is pending we keep kicking in the chosen direction; once the turn is
// spent we go straight again. The body keeps moving forward throughout, which
// is what makes it look like a ball bouncing around the arena.
//
// g++ bounce.cpp -o bounce
#include <unistd.h> // getpid(), read()
#include <fcntl.h>  // fcntl()
#include <stdio.h>  // sscanf(), printf(), fflush()
#include <string.h> // memchr(), memmove()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <thread>   // sleep_for()
#include <chrono>   // milliseconds

// Must match ActionType in src/life.h
enum Action { ACT_NONE = 0, ACT_KICK_LEFT, ACT_KICK_RIGHT, ACT_KICK_BOTH };
// Wire event types as written by src/life.cpp Life::event() (NOT the internal
// EventType enum order): 1 = wave, 2 = collision, 3 = death.
enum WireEvent { WE_WAVE = 1, WE_COLLISION = 2, WE_DEATH = 3 };

constexpr int TURN_STEP_DEG = 10; // engine rotates the heading this much per kick

static void setNonBlock(int fd) { // set non-blocking mode so we never stall on input
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Line-buffered stdin reader, borrowed from sound.cpp.  stdio's scanf() is
// unreliable on a non-blocking fd (an EAGAIN latches the stream's error flag
// and later reads keep failing), so we read raw bytes with read() and parse
// whole '\n'-terminated lines ourselves.
static char   rbuf[4096];
static size_t rlen = 0; // bytes currently held in rbuf

// Drain everything available on stdin, parse every complete line, and report
// whether any of them was a collision event. Draining keeps us in sync with the
// engine, which can emit several events between our actions.
static bool sawCollision() {
    // Read until the pipe is drained (EAGAIN) or full.
    for (;;) {
        if (rlen >= sizeof(rbuf) - 1) rlen = 0; // overflow guard: drop partial
        ssize_t n = read(0, rbuf + rlen, sizeof(rbuf) - 1 - rlen);
        if (n > 0) { rlen += size_t(n); continue; }
        break; // n == 0 (EOF) or n < 0 (EAGAIN/no more data)
    }
    rbuf[rlen] = 0;

    bool collided = false;
    char* start = rbuf;
    char* nl;
    while ((nl = (char*)memchr(start, '\n', rbuf + rlen - start)) != nullptr) {
        *nl = 0;
        int type; long long evtime; int sensor; // time is 64-bit microseconds
        if (3 == sscanf(start, "%d,%lld,%d", &type, &evtime, &sensor)) {
            if (type == WE_COLLISION) collided = true;
        }
        start = nl + 1;
    }
    // Keep any trailing partial line for the next call.
    rlen = rbuf + rlen - start;
    memmove(rbuf, start, rlen);

    return collided;
}

// Pick a fresh random bounce: a number of 10-degree turn steps in a random
// direction. The result is stored as a signed step count (negative = left).
static int randomBounceSteps() {
    // Turn by 90..270 degrees so the new heading clearly differs from the old
    // one (a pure reflection would be ~180). That is 9..27 ten-degree steps.
    const int steps = 9 + (rand() % 19); // 9..27
    const int dir   = (rand() % 2) ? 1 : -1;
    return dir * steps;
}

int main(int argc, char* argv[]) {
    setNonBlock(0);              // 0 = stdin: act without waiting for events
    srand(time(0) - getpid());   // seed the random number generator

    int pendingTurnSteps = 0;    // remaining +/-10 deg kicks to finish a bounce

    while (true) {
        if (sawCollision() && pendingTurnSteps == 0) {
            // Only start a new bounce when not already turning, so repeated
            // collision events while against the wall do not cause jitter.
            pendingTurnSteps = randomBounceSteps();
        }

        int action;
        if (pendingTurnSteps > 0) {         // turning right
            action = ACT_KICK_RIGHT;
            --pendingTurnSteps;
        } else if (pendingTurnSteps < 0) {  // turning left
            action = ACT_KICK_LEFT;
            ++pendingTurnSteps;
        } else {                            // no turn pending: go straight
            action = ACT_KICK_BOTH;
        }

        // Wire format is: time, action, parameter
        printf("0,%d,0\n", action);
        fflush(stdout); // prevent stdio from buffering our action

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // asyncEn runs at 30fps
    }
}
