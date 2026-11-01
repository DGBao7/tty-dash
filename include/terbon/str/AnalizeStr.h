#ifndef ANALYZESTR_H
#define ANALYZESTR_H

#include <vector>
#include <string>

namespace terbon {

std::vector<std::string> split(std::string command);
std::vector<std::string> buildSources(std::vector<std::string> split_command);
std::string check(std::vector<std::string> split_command);

}

#endif
