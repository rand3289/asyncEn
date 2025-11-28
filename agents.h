#pragma once
#include <vector>

void setNonBlock(int fd); // Set non-blocking mode
void spawnAgents(std::vector<int>& inputFds, std::vector<int>& outputFds);
