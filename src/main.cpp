#include "application/Application.hpp"

int main(int argc, char* argv[]) {
    // Parse arguments for headless mode
    bool headless = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--headless") {
            headless = true;
        }
    }

    // createVisualization is the inverse of headless
    bool createVisualization = !headless;

    // Create and run application
    ftree::application::Application app(createVisualization);

    // run() initializes internally if not already initialized
    app.run(100, headless);

    return 0;
}
