#include "application/Application.hpp"
#include "application/StartupPrompt.hpp"
#include "infrastructure/postgres/PostgresSimulationRepository.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <utility>

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "Usage: FamilyTreeHeadless [--new NAME | --load ID | --delete ID] [--years N]\n"
                         "Set FTREE_DATABASE_URL to a PostgreSQL connection string.\n";
            return 0;
        }
    }
    try {
        auto repository = std::make_unique<ftree::infrastructure::postgres::PostgresSimulationRepository>(
            ftree::infrastructure::postgres::PostgresSimulationRepository::connectionStringFromEnvironment());
        auto options = ftree::application::chooseStartupOptions(*repository, argc, argv);
        if (options.showHelp) {
            std::cout << "Usage: FamilyTreeHeadless [--new NAME | --load ID] [--years N]\n"
                         "Set FTREE_DATABASE_URL to a PostgreSQL connection string.\n";
            return 0;
        }
        ftree::application::Application app(nullptr, std::move(repository));
        if (!app.initialize(options.selection)) return 1;
        return app.runHeadless(options.years);
    } catch (const std::exception& error) {
        std::cerr << "FamilyTreeHeadless error: " << error.what() << '\n';
        return 1;
    }
}
