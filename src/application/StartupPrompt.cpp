#include "application/StartupPrompt.hpp"

#include <iostream>
#include <stdexcept>

namespace ftree::application {
namespace {

void printSaves(const std::vector<ports::SimulationSummary>& saves) {
    std::cout << "\nSaved simulations:\n";
    if (saves.empty()) {
        std::cout << "  (no saved simulations)\n";
        return;
    }
    for (const auto& save : saves) {
        std::cout << "  ID " << save.id << " | " << save.name
                  << " | year " << save.currentYear
                  << " | updated " << save.updatedAt << '\n';
    }
}

} // namespace

StartupOptions chooseStartupOptions(
    ports::ISimulationRepository& repository,
    int argc,
    char* argv[])
{
    StartupOptions options;

    bool explicitSelection = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--years" && i + 1 < argc) {
            options.years = std::stoi(argv[++i]);
            if (options.years < 0) throw std::invalid_argument("--years must be non-negative");
        } else if (arg == "--new" && i + 1 < argc) {
            if (explicitSelection) throw std::invalid_argument("Specify only one of --new or --load");
            options.selection.mode = StartupMode::CreateNew;
            options.selection.newSimulationName = argv[++i];
            explicitSelection = true;
        } else if (arg == "--load" && i + 1 < argc) {
            if (explicitSelection) throw std::invalid_argument("Specify only one of --new or --load");
            options.selection.mode = StartupMode::LoadExisting;
            options.selection.simulationId = std::stoll(argv[++i]);
            explicitSelection = true;
        } else if (arg == "--delete" && i + 1 < argc) {
            repository.deleteSimulation(std::stoll(argv[++i]));
        } else if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
            return options;
        } else {
            throw std::invalid_argument("Unknown or incomplete command-line argument: " + arg);
        }
    }
    if (explicitSelection) return options;

    for (;;) {
        const auto saves = repository.listSimulations();
        printSaves(saves);
        std::cout << "\nChoose: [n] new, [d ID] delete, or enter an existing simulation ID: ";
        std::string choice;
        if (!std::getline(std::cin, choice)) throw std::runtime_error("Could not read startup selection");

        if (choice == "n" || choice == "N" || choice == "new") {
            std::cout << "New simulation name: ";
            if (!std::getline(std::cin, options.selection.newSimulationName)
                || options.selection.newSimulationName.empty()) {
                throw std::invalid_argument("Simulation name cannot be empty");
            }
            options.selection.mode = StartupMode::CreateNew;
            return options;
        }

        if (choice.rfind("d ", 0) == 0 || choice.rfind("delete ", 0) == 0) {
            const auto separator = choice.find(' ');
            const auto id = std::stoll(choice.substr(separator + 1));
            std::cout << "Delete simulation " << id << "? [y/N]: ";
            std::string confirmation;
            if (!std::getline(std::cin, confirmation)) {
                throw std::runtime_error("Could not read deletion confirmation");
            }
            if (confirmation == "y" || confirmation == "Y" || confirmation == "yes") {
                repository.deleteSimulation(id);
            }
            continue;
        }

        options.selection.mode = StartupMode::LoadExisting;
        options.selection.simulationId = std::stoll(choice);
        return options;
    }
}

} // namespace ftree::application
