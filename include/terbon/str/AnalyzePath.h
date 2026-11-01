#ifndef ANALYZEPATH_H
#define ANALYZEPATH_H

#include <vector>
#include <string>
#include <QString>

namespace terbon {
namespace Path {
    extern std::string path;

    QString resolve(const QString &requested_path);
    bool setWorkingDirectory(const QString &requested_path , QString *error = nullptr);
}

std::string analyzePath(std::vector<std::string> split_command);

std::string takeTrueParentsPath(std::string path);

}

#endif
