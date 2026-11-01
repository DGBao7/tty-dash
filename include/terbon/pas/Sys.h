#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include "ui/TerminalWidget.h"

namespace terbon {

std::vector<std::string> findSources(std::vector<std::string> sources , std::string path);

void printTree(
    const std::filesystem::path& path ,
    int current_depth ,
    int max_depth ,
    const std::string& prefix ,
    TerminalWidget* terminal
);

}

#endif
