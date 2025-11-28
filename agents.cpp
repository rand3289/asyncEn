#include "asyncen.h" // Args class
#include <unistd.h>
#include <vector>
#include <string>


void spawnAgents(std::vector<int>& inputFds, std::vector<int>& outputFds) {
    const std::vector<std::string>& argv = Args::get(); // argc, argv
    int argc = argv.size();
    inputFds.resize(argc-1);
    outputFds.resize(argc-1);

    for (int i = 1; i < argc; i++) { // argv[0] is asyncEn
        int idx = i-1;

        pid_t pid = fork();
        if (pid == 0) { // Child process
            dup2(inputFds[idx], STDIN_FILENO);
            dup2(outputFds[idx], STDOUT_FILENO);
            // execl() returns -1 when fails
            inputFds[idx] = outputFds[idx] = execl(argv[i].c_str(), argv[i].c_str(), nullptr);
            exit(1); // this runs only if execl() fails
        }
    }
}
