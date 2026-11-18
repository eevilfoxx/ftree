#include "application/Application.hpp"
#include "application/StartupPrompt.hpp"
#include "infrastructure/postgres/PostgresSimulationRepository.hpp"
#include "visualization/SFMLVisualizer.hpp"
#include "visualization/VisualizationConfig.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "Usage: FamilyTree [--new NAME | --load ID | --delete ID]\n"
                         "Set FTREE_DATABASE_URL to a PostgreSQL connection string.\n";
            return 0;
        }
    }
    try {
        auto repository = std::make_unique<ftree::infrastructure::postgres::PostgresSimulationRepository>(
            ftree::infrastructure::postgres::PostgresSimulationRepository::connectionStringFromEnvironment());
        auto options = ftree::application::chooseStartupOptions(*repository, argc, argv);
        if (options.showHelp) {
            std::cout << "Usage: FamilyTree [--new NAME | --load ID] [--years N]\n"
                         "Set FTREE_DATABASE_URL to a PostgreSQL connection string.\n";
            return 0;
        }

        auto visualizer = std::make_unique<ftree::visualization::SFMLVisualizer>(
            ftree::visualization::VisualizationConfig::WINDOW_WIDTH,
            ftree::visualization::VisualizationConfig::WINDOW_HEIGHT);
        ftree::application::Application app(std::move(visualizer), std::move(repository));
        if (!app.initialize(options.selection)) return 1;
        return app.runVisual();
    } catch (const std::exception& error) {
        std::cerr << "FamilyTree error: " << error.what() << '\n';
        return 1;
    }
}
