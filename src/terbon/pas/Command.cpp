
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include <windows.h>
#include <cstdio>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <algorithm>
#include <QtCore/QFile>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <functional>

#include "pas/Command.h"
#include "str/AnalyzePath.h"
#include "ui/TerminalWidget.h"
#include "str/AnalizeStr.h"
#include "pas/Sys.h"
#include "str/clangCode.h"
#include "ui/StatusBar.h"

namespace terbon {
namespace {

void _run_tool_process(const QString &program , const QStringList &arguments , const QString &working_directory , TerminalWidget *terminal , StatusBar *status_bar , const QString &failure_message , const std::function<void(bool)> &finished_callback = {}) {
    auto *process = new QProcess(terminal);
    process->setProgram(program);
    process->setArguments(arguments);
    process->setWorkingDirectory(working_directory);
    process->setProcessChannelMode(QProcess::MergedChannels);
    terminal->SetRunningProcess(process);
    QObject::connect(process , &QProcess::readyRead , process , [process , terminal]() {
        const QString output = QString::fromUtf8(process->readAll());
        if (!output.isEmpty()) terminal->Print(output);
    });
    QObject::connect(process , qOverload<int , QProcess::ExitStatus>(&QProcess::finished) , process , [process , terminal , status_bar , finished_callback](int exit_code , QProcess::ExitStatus) {
        const QString remaining = QString::fromUtf8(process->readAll());
        if (!remaining.isEmpty()) terminal->Print(remaining);
        const bool succeeded = exit_code == 0;
        if (finished_callback) finished_callback(succeeded);
        terminal->SetRunningProcess(nullptr);
        status_bar->UpdateStatus(QStringLiteral("● Ready"));
        process->deleteLater();
    });
    QObject::connect(process , &QProcess::errorOccurred , process , [process , terminal , status_bar , failure_message](QProcess::ProcessError error) {
        if (error != QProcess::FailedToStart) return;
        terminal->SetRunningProcess(nullptr);
        terminal->Print(failure_message);
        status_bar->UpdateStatus(QStringLiteral("● Ready"));
        process->deleteLater();
    });
    process->start();
}

void exeOpe(const std::string &target , const std::string &stuff , TerminalWidget *terminal)
{
    bool opened = false;
    if (target == "app")
    {
        opened = QProcess::startDetached(QString::fromStdString(stuff) , {} , QString::fromStdString(Path::path));
    }
    else if (target == "url")
    {
        const QUrl url = QUrl::fromUserInput(QString::fromStdString(stuff));
        const QString scheme = url.scheme().toLower();
        opened = url.isValid() && (scheme == QStringLiteral("http") || scheme == QStringLiteral("https") || scheme == QStringLiteral("mailto"))
            && QDesktopServices::openUrl(url);
    }
    else if (target == "fil")
    {
        const QString path = Path::resolve(QString::fromStdString(stuff));
        const QFileInfo info(path);
        opened = info.isFile() && QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
    }
    else if (target == "fol")
    {
        const QString path = Path::resolve(QString::fromStdString(stuff));
        const QFileInfo info(path);
        opened = info.isDir() && QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
    }
    if (!opened && terminal) terminal->Print("Could not open the requested target");
}

void exeCre(std::string target ,
            std::vector<std::string> split_command
       )
{
    const QString destination = Path::resolve(QString::fromStdString(split_command[2]));

    if (target == "fil")
    {
        QFile file(destination);
        if (file.open(QIODevice::WriteOnly)) file.close();
    }

    else if (target == "fol")
    {
        QDir().mkdir(destination);
    }
}

void exeDel(std::string target ,
            std::vector<std::string> split_command
       )
{
    const QString target_path = Path::resolve(QString::fromStdString(split_command[2]));

    if (target == "fil")
    {
        QFile::remove(target_path);
    }
    else if (target == "fol")
    {
        QDir().rmdir(target_path);
    }
}

void exeSea(std::string target ,
            std::vector<std::string> split_command ,
            TerminalWidget* terminal)
{
    bool found = false;

    std::string name = split_command[2];
    std::string path = Path::path;

    if (target == "fil")
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            std::string filename =
                entry.path().filename().string();

            if (
                entry.is_regular_file() &&
                filename.find(name) != std::string::npos
            )
            {
                terminal -> Print(
                    QString::fromStdString(
                        entry.path().string()
                    )
                );

                found = true;
            }
        }
    }
    else if (target == "fol")
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            std::string filename =
                entry.path().filename().string();

            if (
                entry.is_directory() &&
                filename.find(name) != std::string::npos
            )
            {
                terminal -> Print(
                    QString::fromStdString(
                        entry.path().string()
                    )
                );

                found = true;
            }
        }
    }

    if (!found)
    {
        terminal -> Print(
            "Error in searching file or cannot be found"
        );
    }
}

void exeCle(TerminalWidget* terminal)
{
    terminal -> ClearTerminal();
}


void exeRun(std::string target ,
            std::vector<std::string> split_command ,
            TerminalWidget* terminal ,
            StatusBar* status_bar
       )
{
    status_bar -> UpdateStatus(
            QString::fromStdString(split_command[2]) +
            "      ● Running"
       );

    const QString working_directory = QString::fromStdString(Path::path);
    const std::string file = Path::resolve(QString::fromStdString(split_command[2])).toStdString();

    if (target == "cpp")
    {
        std::string exe = file.substr(0 , file.find_last_of(".")) + ".exe";

        QProcess* compile = new QProcess(terminal);

        compile->setProgram("g++");

        compile->setWorkingDirectory(working_directory);

        compile->setArguments(
            {
                QString::fromStdString(file),
                "-o",
                QString::fromStdString(exe)
            }
        );

        compile->setProcessChannelMode(QProcess::MergedChannels);

        QObject::connect(
            compile ,
            &QProcess::readyRead ,
            [compile , terminal]()
            {
                QString output = QString::fromUtf8(
                    compile->readAll()
                );

                if (!output.isEmpty())
                {
                    terminal->Print(output);
                }
            }
        );

        QObject::connect(
            compile ,
            QOverload<int , QProcess::ExitStatus>::of(&QProcess::finished) ,
            [compile , terminal , exe , status_bar , working_directory]()
            {
                if (compile->exitCode() != 0)
                {
                    terminal->Print("Build failed due to g++ error or various purposes");
                    terminal -> NewPrompt();
                    compile->deleteLater();
                    status_bar -> UpdateStatus("● Ready");
                    if (compile->exitCode() != 0)

                    return;
                }

                terminal->Print("Build successful");

                QProcess* run = new QProcess(terminal);

                run->setProgram(
                    QString::fromStdString(exe)
                );
                run->setWorkingDirectory(working_directory);

                run->setProcessChannelMode(QProcess::MergedChannels);

                terminal->SetRunningProcess(run);

                QObject::connect(
                    run ,
                    &QProcess::readyRead ,
                    [run , terminal]()
                    {
                        QString output = QString::fromUtf8(
                            run->readAll()
                        );

                        if (!output.isEmpty())
                        {
                            terminal->Print(output);
                        }
                    }
                );

                QObject::connect(
                    run ,
                    QOverload<int , QProcess::ExitStatus>::of(&QProcess::finished) ,
                    [run , terminal , status_bar](int , QProcess::ExitStatus)
                    {
                        terminal->SetRunningProcess(nullptr);
                        terminal->Print("Process finished");
                        status_bar -> UpdateStatus("● Ready");
                        terminal -> NewPrompt();

                        run->deleteLater();
                    }
                );

                QObject::connect(
                    run ,
                    &QProcess::errorOccurred ,
                    [run , terminal , status_bar](QProcess::ProcessError)
                    {
                        terminal->SetRunningProcess(nullptr);
                        terminal->Print("File can not be openned");
                        status_bar -> UpdateStatus("● Ready");

                        run->deleteLater();
                    }
                );

                run->start();

                compile->deleteLater();
            }
        );

        QObject::connect(
            compile ,
            &QProcess::errorOccurred ,
            [compile , terminal , status_bar](QProcess::ProcessError)
            {
                terminal->Print("G++ can not be started or should udate");
                status_bar -> UpdateStatus("● Ready");
                compile->deleteLater();
            }
        );

        compile->start();
    }

    else if (target == "py")
    {
        QProcess* process = new QProcess(terminal);

        process->setProgram("python");

        process -> setWorkingDirectory(
            QString::fromStdString(Path::path)
        );

        process->setArguments(
            {
                QString::fromStdString(file)
            }
        );

        process->setProcessChannelMode(QProcess::MergedChannels);

        terminal->SetRunningProcess(process);

        QObject::connect(
            process ,
            &QProcess::readyRead ,
            [process , terminal]()
            {
                QString output = QString::fromUtf8(
                    process->readAll()
                );

                if (!output.isEmpty())
                {
                    terminal->Print(output);
                }
            }
        );

        QObject::connect(
            process ,
            QOverload<int , QProcess::ExitStatus>::of(&QProcess::finished) ,
            [process , terminal , status_bar](int , QProcess::ExitStatus)
            {
                terminal->SetRunningProcess(nullptr);
                terminal->Print("Process finished");
                status_bar -> UpdateStatus("● Ready");
                terminal -> NewPrompt();

                process->deleteLater();
            }
        );

        QObject::connect(
            process ,
            &QProcess::errorOccurred ,
            [process , terminal , status_bar](QProcess::ProcessError)
            {
                terminal->SetRunningProcess(nullptr);
                terminal->Print("Python 3.x.x can not be started");
                status_bar -> UpdateStatus("● Ready");

                process->deleteLater();
            }
        );

        process->start();
    }

    else if (target == "js")
    {
        QProcess* process = new QProcess(terminal);

        process -> setProgram("node");

        process -> setWorkingDirectory(
            QString::fromStdString(Path::path)
        );

        process -> setArguments(
            {
                QString::fromStdString(file)
            }
        );

        process -> setProcessChannelMode(QProcess::MergedChannels);

        terminal -> SetRunningProcess(process);

        QObject::connect(
            process ,
            &QProcess::readyRead ,
            [process , terminal]()
            {
                QString output = QString::fromUtf8(
                    process -> readAll()
                );

                if (!output.isEmpty())
                {
                    terminal -> Print(output);
                }
            }
        );

        QObject::connect(
            process ,
            QOverload<int , QProcess::ExitStatus>::of(&QProcess::finished) ,
            [process , terminal , status_bar](int , QProcess::ExitStatus)
            {
                terminal -> SetRunningProcess(nullptr);
                terminal -> Print("Process finished");
                status_bar -> UpdateStatus("● Ready");
                terminal -> NewPrompt();
                process -> deleteLater();
            }
        );

        QObject::connect(
            process ,
            &QProcess::errorOccurred ,
            [process , terminal , status_bar](QProcess::ProcessError)
            {
                terminal -> SetRunningProcess(nullptr);
                terminal -> Print("Node.js can not be started");
                status_bar -> UpdateStatus("● Ready");
                process -> deleteLater();
            }
        );

        process -> start();
    }

    else if (target == "exe")
    {
        if (!std::filesystem::exists(file))
        {
            terminal->Print("File can not be found");
            status_bar -> UpdateStatus("● Ready");

            return;
        }

        QProcess* process = new QProcess(terminal);

        process->setProgram(
            QString::fromStdString(file)
        );

        process -> setWorkingDirectory(
            QString::fromStdString(Path::path)
        );

        process->setProcessChannelMode(QProcess::MergedChannels);

        terminal->SetRunningProcess(process);

        QObject::connect(
            process ,
            &QProcess::readyRead ,
            [process , terminal]()
            {
                QString output = QString::fromUtf8(
                    process->readAll()
                );

                if (!output.isEmpty())
                {
                    terminal->Print(output);
                }
            }
        );

        QObject::connect(
            process ,
            QOverload<int , QProcess::ExitStatus>::of(&QProcess::finished) ,
            [process , terminal , status_bar](int , QProcess::ExitStatus)
            {
                terminal->SetRunningProcess(nullptr);
                terminal->Print("Process finished");
                terminal -> NewPrompt();
                status_bar -> UpdateStatus("● Ready");

                process->deleteLater();
            }
        );

        process->start();
    }
}

//

void exeBui(std::string target ,
            std::vector<std::string> split_command ,
            TerminalWidget* terminal ,
            StatusBar* status_bar
       )
{
    status_bar -> UpdateStatus("● Building");

    const QString working_directory = QString::fromStdString(Path::path);

    const QString main_source = Path::resolve(QString::fromStdString(split_command[2]));

    std::vector<std::string> source_patterns = buildSources(split_command);

    std::vector<std::string> found_sources = findSources(
        source_patterns ,
        working_directory.toStdString()
    );

    QStringList arguments;

    arguments << main_source;

    for (const std::string& source : found_sources)
    {
        arguments << QString::fromStdString(source);
    }

    if (target != "cpp") return;
    arguments << "-o" << QDir(working_directory).filePath(QStringLiteral("a.exe"));
    status_bar->UpdateStatus(QStringLiteral("● Building"));
    _run_tool_process(QStringLiteral("g++") , arguments , working_directory , terminal , status_bar , QStringLiteral("Cannot start g++") , [terminal](bool succeeded) {
        terminal->Print(succeeded ? QStringLiteral("Build successful") : QStringLiteral("Build failed due to g++ error or various purposes"));
    });
}

void exeAna(std::string target ,
            std::vector<std::string> split_command ,
            TerminalWidget* terminal
       )
{
    if (target == "fil")
    {
        const std::string file = Path::resolve(QString::fromStdString(split_command[2])).toStdString();

        std::ifstream source(file);

        if (!source.is_open())
        {
            terminal->Print("File can not be found");

            return;
        }

        std::string line;

        int lines = 0;
        int includes = 0;
        int classes = 0;
        int functions = 0;

        while (std::getline(source , line))
        {
            lines++;

            if (line.find("#include") != std::string::npos)
            {
                includes++;
            }

            if (line.find("class ") != std::string::npos)
            {
                classes++;
            }

            if (
                line.find("(") != std::string::npos &&
                line.find(")") != std::string::npos &&
                line.find(";") == std::string::npos &&
                line.find("if") == std::string::npos &&
                line.find("for") == std::string::npos &&
                line.find("while") == std::string::npos &&
                line.find("switch") == std::string::npos
            )
            {
                functions++;
            }
        }

        source.close();

        terminal->Print(
            "File      : " +
            QString::fromStdString(split_command[2])
        );

        terminal->Print(
            "Lines     : " +
            QString::number(lines)
        );

        terminal->Print(
            "Includes  : " +
            QString::number(includes)
        );

        terminal->Print(
            "Classes   : " +
            QString::number(classes)
        );

        terminal->Print(
            "Functions : " +
            QString::number(functions)
        );
    }
}

void exeFor(std::string target ,
            std::vector<std::string> split_command ,
            TerminalWidget* terminal ,
            StatusBar* status_bar
       )
{
    const QString base = QString::fromStdString(Path::path);
    const QString file = Path::resolve(QString::fromStdString(split_command[2]));
    if (!QFileInfo::exists(file)) {
        terminal->Print(QStringLiteral("File not found"));
        return;
    }

    status_bar->UpdateStatus(QStringLiteral("● Formatting"));
    if (target == "cpp") {
        const QString style_path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral(".clang-format"));
        const QString style = QFileInfo::exists(style_path) ? QStringLiteral("--style=file:%1").arg(QDir::toNativeSeparators(style_path)) : QStringLiteral("--style=file");
        _run_tool_process(QStringLiteral("clang-format") , {QStringLiteral("-i") , style , file} , base , terminal , status_bar , QStringLiteral("Cannot start clang-format") , [terminal , file](bool succeeded) {
            if (succeeded) clangCpp(file.toStdString());
            terminal->Print(succeeded ? QStringLiteral("Format successful") : QStringLiteral("Format failed"));
        });
    } else if (target == "py") {
        _run_tool_process(QStringLiteral("python") , {QStringLiteral("-m") , QStringLiteral("black") , file} , base , terminal , status_bar , QStringLiteral("Cannot start black") , [terminal , file](bool succeeded) {
            if (succeeded) clangPy(file.toStdString());
            terminal->Print(succeeded ? QStringLiteral("Format successful") : QStringLiteral("Format failed"));
        });
    }
}

void exePwd(TerminalWidget* terminal)
{
    std::string path = Path::path;

    terminal -> Print(
        QString::fromStdString(path)
    );
}

void exeCd(std::vector<std::string> split_command , TerminalWidget* terminal)
{
    if (split_command.size() < 2)
    {
        terminal -> Print("Missing path");

        return;
    }

    QString error;
    const QString requested_path = QString::fromStdString(analyzePath(split_command));
    if (!Path::setWorkingDirectory(requested_path , &error)) {
        terminal->Print(error);
    }
}

void exeDir(TerminalWidget* terminal)
{
    try
    {
        std::vector<std::filesystem::directory_entry> folders;
        std::vector<std::filesystem::directory_entry> files;

        for (const auto& entry : std::filesystem::directory_iterator(Path::path))
        {
            if (entry.is_directory())
            {
                folders.push_back(entry);
            }
            else if (entry.is_regular_file())
            {
                files.push_back(entry);
            }
        }

        std::sort(
            folders.begin() ,
            folders.end() ,
            [](const auto& a , const auto& b)
            {
                return a.path().filename().string() <
                       b.path().filename().string();
            }
        );

        std::sort(
            files.begin() ,
            files.end() ,
            [](const auto& a , const auto& b)
            {
                return a.path().filename().string() <
                       b.path().filename().string();
            }
        );

        terminal -> Print(
            QString::fromStdString(
                "\nDirectory: " + Path::path
            )
        );

        terminal -> Print("\nFolders");
        terminal -> Print("--------------------------------");

        for (const auto& folder : folders)
        {
            terminal -> Print(
                QString::fromStdString(
                    folder.path().filename().string()
                )
            );
        }

        terminal -> Print("\nFiles");
        terminal -> Print("--------------------------------");

        for (const auto& file : files)
        {
            std::uintmax_t size = std::filesystem::file_size(file);

            std::string name =
                file.path().filename().string();

            std::string output =
                name;

            if (name.length() < 20)
            {
                output += std::string(20 - name.length() , ' ');
            }

            if (size < 1024)
            {
                output +=
                    std::to_string(size) + " B";
            }
            else
            {
                double sizeKB =
                    static_cast<double>(size) / 1024.0;

                output +=
                    std::to_string(
                        static_cast<int>(sizeKB)
                    ) + " KB";
            }

            terminal -> Print(
                QString::fromStdString(output)
            );
        }

        terminal -> Print(
            QString::fromStdString(
                "\n" +
                std::to_string(folders.size()) +
                " folders, " +
                std::to_string(files.size()) +
                " files"
            )
        );
    }
    catch (const std::filesystem::filesystem_error&)
    {
        terminal -> Print("Cannot access directory");
    }
}

void exeTre(std::vector<std::string> split_command , TerminalWidget* terminal)
{
    if (split_command.size() < 2)
    {
        terminal -> Print("Missing depth");

        return;
    }

    int depth;

    try
    {
        depth = std::stoi(split_command[1]);
    }
    catch (...)
    {
        terminal -> Print("Invalid depth");

        return;
    }

    if (depth < 1)
    {
        terminal -> Print("Depth must be greater than 0");

        return;
    }

    terminal -> Print(
        QString::fromStdString(Path::path)
    );

    printTree(
        Path::path ,
        0 ,
        depth ,
        "" ,
        terminal
    );
}

void exeHelp(
   std::vector<std::string> split_command ,
   TerminalWidget* terminal
)
{
    QFile file(":/configs/commands.json");

    if (!file.open(QIODevice::ReadOnly))
    {
        terminal -> Print(
            "Cannot open commands configuration"
        );

        return;
    }

    QByteArray content = file.readAll();

    nlohmann::json data;

    try
    {
        data = nlohmann::json::parse(
            content.constData()
        );
    }
    catch (...)
    {
        terminal -> Print(
            "Invalid commands configuration"
        );

        return;
    }

    if (split_command.size() == 1)
    {
        terminal -> Print("");

        terminal -> Print(
            "===================== TerBon Help ====================="
        );

        terminal -> Print("");

        for (const auto& command : data.items())
        {
            std::string key = command.key();
            std::string name =
                command.value()["name"].get<std::string>();

            std::string note =
                command.value()["note"].get<std::string>();

            std::string line =
                "  " +
                key;

            while (line.length() < 8)
            {
                line += " ";
            }

            line += name;

            while (line.length() < 35)
            {
                line += " ";
            }

            line += note;

            terminal -> Print(
                QString::fromStdString(line)
            );
        }

        terminal -> Print("");

        return;
    }

    if (split_command.size() > 2)
    {
        terminal -> Print(
            "Usage: help <command>"
        );

        return;
    }

    std::string target = split_command[1];

    if (!data.contains(target))
    {
        terminal -> Print(
            "Command not found: " +
            QString::fromStdString(target)
        );

        return;
    }

    std::string name =
        data[target]["name"].get<std::string>();

    std::string note =
        data[target]["note"].get<std::string>();

    terminal -> Print("");

    terminal -> Print(
        "===================== " +
        QString::fromStdString(name) +
        " ====================="
    );

    terminal -> Print("");

    terminal -> Print(
        QString::fromStdString(note)
    );

    terminal -> Print("");
}

} // namespace

void execute(
   std::vector<std::string> split_command ,
   TerminalWidget* terminal ,
   StatusBar* status_bar
)
{
    if (split_command.empty()) return;
    std::string action = split_command[0];

    if (action == "cle")
    {
        exeCle(terminal);

        return;
    }
    else if (action == "cd")
    {
        std::string old_path = Path::path;

        exeCd(split_command , terminal);

        if (old_path != Path::path)
        {
            status_bar -> UpdatePath();
        }

        return;
    }
    else if (action == "pwd")
    {
        exePwd(terminal);

        return;
    }
    else if (action == "dir")
    {
        exeDir(terminal);

        return;
    }
    else if (action == "help")
    {
        exeHelp(split_command , terminal);

        return;
    }

    std::string target = split_command[1];

    if (action == "ope")
    {
        exeOpe(target , split_command[2] , terminal);
    }
    else if (action == "cre")
    {
        exeCre(target , split_command);
    }
    else if (action == "del")
    {
        exeDel(target , split_command);
    }
    else if (action == "sea")
    {
        exeSea(target , split_command , terminal);
    }
    else if (action == "run")
    {
        exeRun(target , split_command , terminal , status_bar);
        // terminal->NewPrompt();
    }
    else if (action == "bui")
    {
        exeBui(target , split_command , terminal , status_bar);
    }
    else if (action == "ana")
    {
        exeAna(target , split_command , terminal);
    }
    else if (action == "for")
    {
        exeFor(target , split_command , terminal , status_bar);
    }
    else if (action == "tre")
    {
        exeTre(split_command , terminal);
    }
    else if (action == "the")
    {

    }

    return;
}

}
