#include "application/Application.hpp"
#include "visualization/SFMLVisualizer.hpp"
#include "visualization/VisualizationConfig.hpp"

#include <memory>
#include <string>
#include <utility>

int main(int argc, char* argv[])
{
    bool headless = false;
    int years = 100;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--headless") {
            headless = true;
        } else if (arg == "--years" && i + 1 < argc) {
            years = std::stoi(argv[++i]);
        }
    }

    std::unique_ptr<ftree::application::IVisualizer> visualizer;

    if (!headless) {
        visualizer = std::make_unique<ftree::visualization::SFMLVisualizer>(
            ftree::visualization::VisualizationConfig::WINDOW_WIDTH,
            ftree::visualization::VisualizationConfig::WINDOW_HEIGHT
        );
    }

    ftree::application::Application app(std::move(visualizer));

    if (!app.initialize()) {
        return 1;
    }

    return headless
        ? app.runHeadless(years)
        : app.runVisual();
}
