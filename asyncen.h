#pragma once
#include <vector>
#include <string>

class Args {
    static std::vector<std::string> args;
public:

    static void init(int argc, char* argv[]){
        for(int i = 0; i < argc; ++i){
            args.push_back(argv[i]);
        }
    }

    static const std::vector<std::string>& get() {
        return args;
    }
};
