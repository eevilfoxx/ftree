#include "application/Application.hpp"

int main(int argc, char* argv[]) {
    // Parse arguments for headless mode
    bool headless = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--headless") {
            headless = true;
        }
    }
    
    // Create and run application
    ftree::application::Application app(headless);
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    // Run simulation for 100 years
    app.run(100, headless);
    
    return 0;
}
