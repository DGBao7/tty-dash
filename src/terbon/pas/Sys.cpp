#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include "ui/TerminalWidget.h"
#include "pas/Sys.h"

namespace terbon {

class TerminalWidget;

std::vector<std::string> findSources(
    std::vector<std::string> sources ,
    std::string path)
{
    std::vector<std::string> result;

    for (const std::string& source : sources)
    {
        size_t slash = source.find_last_of("/\\");

        std::string folder = source.substr(0 , slash);
        std::string name = source.substr(slash + 1);

        size_t star = name.find("*");

        std::string before = name.substr(0 , star);
        std::string after = name.substr(star + 1);

        std::string full_folder = path + folder;

        for (const auto& entry : std::filesystem::directory_iterator(full_folder))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            std::string filename = entry.path().filename().string();

            if (
                filename.size() >= before.size() + after.size() &&
                filename.substr(0 , before.size()) == before &&
                filename.substr(filename.size() - after.size()) == after
            )
            {
                result.push_back(folder + "/" + filename);
            }
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
