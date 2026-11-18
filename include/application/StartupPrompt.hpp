#pragma once

#include "application/Application.hpp"

#include <vector>
#include <string>

namespace ftree::application {

struct StartupOptions {
    StartupSelection selection;
    int years = 100;
    bool showHelp = false;
};

StartupOptions chooseStartupOptions(
    ports::ISimulationRepository& repository,
    int argc,
    char* argv[]
);

} // namespace ftree::application
