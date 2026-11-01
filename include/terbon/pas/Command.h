#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

#include "ui/TerminalWidget.h"
#include "ui/StatusBar.h"

namespace terbon {

void execute(
    std::vector<std::string> split_command ,
    TerminalWidget* terminal ,
    StatusBar* status_bar
);

}

#endif
