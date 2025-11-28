#include "asyncen.h" // Args class
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string>


void setNonBlock(int fd) { // Set non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


void spawnAgents(const std::vector<std::string>& argv, std::vector<int>& inputFds, std::vector<int>& outputFds) {
    inputFds.resize(argv.size());
    outputFds.resize(argv.size());

    for (int i=0; i < argv.size(); ++i) {
        int readPipe[2], writePipe[2];
        if (pipe(readPipe) == -1 || pipe(writePipe) == -1) {
            perror("pipe");
            continue; // TODO: leaks handles
        }

        pid_t pid = fork();
        if (pid == 0) { // Child process
            dup2(readPipe[0], STDIN_FILENO);
            dup2(writePipe[1], STDOUT_FILENO);
            close( readPipe[1]);
            close(writePipe[0]);

            // execl() returns -1 when fails
            inputFds[i] = outputFds[i] = execl(argv[i].c_str(), argv[i].c_str(), nullptr);
            exit(1); // exit that process if execl() fails
        } else {
            inputFds[i]  = readPipe[1];
            outputFds[i] = writePipe[0];
            close( readPipe[0]);
            close(writePipe[1]);
        }
    } // for
}


void spawnAgents(std::vector<int>& inputFds, std::vector<int>& outputFds) {
    std::vector<std::string> argv( Args::get() ); // copy command line parameters
    argv.erase(argv.begin()); // erase argv[0] which is asyncEn
    spawnAgents(argv, inputFds, outputFds);
}
