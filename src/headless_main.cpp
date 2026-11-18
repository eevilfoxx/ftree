#include "application/Application.hpp"

#include <string>

int main(int argc, char* argv[])
{
    int years = 100;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--years" && i + 1 < argc) {
            years = std::stoi(argv[++i]);
        }
    }

    ftree::application::Application app;
    if (!app.initialize()) {
        return 1;
    }

    return app.runHeadless(years);
}
