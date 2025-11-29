// A randomly moving simplest example agent for asyncEn environment
// g++ easya.cpp -o easya
#include <cstdlib>  // rand()
#include <unistd.h> // getpid()
#include <thread>   // sleep_for()
#include <stdio.h>
#include <chrono>


int main(int argc, char* argv[]) {
    srand( time(0) - getpid() );     // seed the random number generator
    while(true){                     //
        printf("%d", 1+(rand()%3) ); // randomly print numbers 1 or 2 or 3
        fflush(stdout);              // prevent stdio from buffering
        std::this_thread::sleep_for( std::chrono::milliseconds(50) ); // asyncEn runs at 30fps
    }
}
