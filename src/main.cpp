#include "application/Application.hpp"

#include <string>
#include <stdexcept>

int main(int argc, char* argv[]) {
    bool headless = false;
    int years = 100;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--headless") {
            headless = true;
        }
        else if (arg == "--years" && i + 1 < argc) {
            years = std::stoi(argv[++i]);
        }
    }

    ftree::application::Application app(true);

    if (headless) {
        return app.runHeadless(years);
    }

    return app.runVisual();
}