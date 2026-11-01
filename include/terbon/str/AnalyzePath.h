#ifndef ANALYZEPATH_H
#define ANALYZEPATH_H

#include <vector>
#include <string>

namespace terbon {
namespace Path {
    extern std::string path;
}

std::string analyzePath(std::vector<std::string> split_command);

std::string takeTrueParentsPath(std::string path);

}

#endif
