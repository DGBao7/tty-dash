#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

#include "ui/TerminalWidget.h"
#include "pas/Sys.h"
#include "str/AnalyzePath.h"

namespace terbon {

class TerminalWidget;

std::vector<std::string> findSources(
    std::vector<std::string> sources ,
    std::string path)
{
    std::vector<std::string> result;
    const QString current_directory = QString::fromStdString(path);

    for (const std::string &source : sources) {
        const QString pattern = QDir::fromNativeSeparators(QString::fromStdString(source));
        const qsizetype separator = pattern.lastIndexOf(QLatin1Char('/'));
        const QString folder = separator < 0 ? QStringLiteral(".") : pattern.left(separator);
        const QString file_pattern = separator < 0 ? pattern : pattern.mid(separator + 1);
        const bool drive_qualified = folder.size() >= 2 && folder.at(1) == QLatin1Char(':');
        const QString resolved_folder = QDir::isAbsolutePath(folder) || drive_qualified
            ? Path::resolve(folder)
            : QDir::cleanPath(QDir(current_directory).absoluteFilePath(folder));
        const QDir directory(resolved_folder);
        const QFileInfoList matches = directory.entryInfoList(
            QStringList{file_pattern} ,
            QDir::Files | QDir::NoDotAndDotDot ,
            QDir::Name
        );

        for (const QFileInfo &match : matches) {
            result.push_back(QDir::toNativeSeparators(match.absoluteFilePath()).toStdString());
        }
    }

    return result;
}

void printTree(
    const std::filesystem::path& path ,
    int current_depth ,
    int max_depth ,
    const std::string& prefix ,
    TerminalWidget* terminal
)
{
    if (current_depth >= max_depth)
    {
        return;
    }

    std::vector<std::filesystem::directory_entry> entries;

    try
    {
        for (const auto& entry :
             std::filesystem::directory_iterator(path))
        {
            entries.push_back(entry);
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return;
    }

    std::sort(
        entries.begin() ,
        entries.end() ,
        [](const auto& a , const auto& b)
        {
            if (a.is_directory() != b.is_directory())
            {
                return a.is_directory() > b.is_directory();
            }

            return a.path().filename().string() <
                   b.path().filename().string();
        }
    );

    for (size_t i = 0; i < entries.size(); i++)
    {
        bool last = i == entries.size() - 1;

        std::string branch =
            last ? "└── " : "├── ";

        terminal -> Print(
            QString::fromStdString(
                prefix +
                branch +
                entries[i].path().filename().string()
            )
        );

        if (entries[i].is_directory())
        {
            printTree(
                entries[i].path() ,
                current_depth + 1 ,
                max_depth ,
                prefix + (last ? "    " : "│   ") ,
                terminal
            );
        }
    }
}

}
