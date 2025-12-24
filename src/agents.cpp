#include "asyncen.h" // Args class
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string>

// TODO: keep the list of processes and kill them all when exiting

static void setNonBlock(int fd) { // Set non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


static void spawnAgents(const std::vector<std::string>& argv, std::vector<int>& inputFds, std::vector<int>& outputFds) {
    inputFds.resize(argv.size());
    outputFds.resize(argv.size());

    for (int i=0; i < argv.size(); ++i) {
        int readPipe[2], writePipe[2];
        if (pipe(readPipe) == -1) {
            perror("pipe");
            continue;
        }
        if (pipe(writePipe) == -1){
            close(readPipe[0]);
            close(readPipe[1]);
            perror("pipe");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) { // child process
            close( readPipe[1]);
            close(writePipe[0]);
            dup2( readPipe[0],  STDIN_FILENO);
            dup2(writePipe[1], STDOUT_FILENO);

            // execl() returns -1 when fails
            // TODO: how do you signal failure here?  Another output array?
            execl(argv[i].c_str(), argv[i].c_str(), nullptr);
            perror("execl");
            fflush(stderr);
            exit(1); // exit that process if execl() fails
        } else { // parent process
            close( readPipe[0]);
            close(writePipe[1]);
            setNonBlock( readPipe[1]); // do not wait on slow agents
            setNonBlock(writePipe[0]);
            inputFds[i]  =  readPipe[1];
            outputFds[i] = writePipe[0];
        }
    } // for
}


void spawnAgents(std::vector<int>& inputFds, std::vector<int>& outputFds) {
    std::vector<std::string> argv( Args::get() ); // copy command line parameters
    argv.erase(argv.begin()); // erase argv[0] which is asyncEn
    spawnAgents(argv, inputFds, outputFds);
}
