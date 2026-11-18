
#pragma once

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"

#include <vector>

namespace ftree::application {

struct VisualizationState {
    bool isPaused = false;
    bool autoAdvance = false;
    int currentYear = 0;
};

enum class VisualizationCommand {
    PauseResume,
    NextYear,
    ToggleAutoAdvance,
    IncreaseSpeed,
    DecreaseSpeed
};

class IVisualizer {
public:
    virtual ~IVisualizer() = default;

    virtual void setData(
        std::vector<presentation::PersonDisplay> people,
        std::vector<presentation::FamilyDisplay> families
    ) = 0;

    virtual void setState(
        const VisualizationState& state
    ) = 0;

    virtual bool isOpen() const = 0;

    virtual void processEvents() = 0;

    virtual std::vector<VisualizationCommand> consumeCommands() = 0;

    virtual void render() = 0;
};

}
