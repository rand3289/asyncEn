// easy agent
#include <cstdlib>  // rand()
#include <unistd.h> // getpid()
#include <thread>   // sleep_for()
#include <stdio.h>
#include <chrono>


int main(int argc, char* argv[]) {
    srand( time(0) - getpid() );
    while(true){
        printf("%d", 1+(rand()%3) );
        fflush(stdout);
        std::this_thread::sleep_for( std::chrono::milliseconds(50) );
    }
}
