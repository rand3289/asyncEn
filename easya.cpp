// A simple randomly moving example agent for asyncEn environment
// g++ easya.cpp -o easya
#include <unistd.h> // getpid()
#include <fcntl.h>  // fcntl()
#include <stdio.h>  // scanf(), printf(), fprintf(), flush()
#include <cstdlib>  // rand()
#include <thread>   // sleep_for()
#include <chrono>   // milliseconds

static void setNonBlock(int fd) { // Set non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]) {
    setNonBlock(0); // 0=stdin       // act without waiting for events
    srand( time(0) - getpid() );     // seed the random number generator
    while(true){                     //
        int event, time, param;
        ssize_t rd = scanf("%d,%d,%d\n", &event, &time, &param);
        if(3==rd){ // print to console through stderr for debugging
            fprintf(stderr, "%d,%d,%d\t", event, time/1000, param);
            fflush(stderr);
        }

        // the format is time, action, parameter
        printf("0,%d,0\n", 1+(rand()%3) ); // randomly print action 1 or 2 or 3
        fflush(stdout);                    // prevent stdio from buffering

        std::this_thread::sleep_for( std::chrono::milliseconds(50) ); // asyncEn runs at 30fps
    }
}
