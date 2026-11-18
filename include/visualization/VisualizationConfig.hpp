
#pragma once

namespace ftree::visualization {

struct VisualizationConfig {
    // Window settings
    static constexpr unsigned int WINDOW_WIDTH = 1024;
    static constexpr unsigned int WINDOW_HEIGHT = 768;
    static constexpr unsigned int FRAMERATE_LIMIT = 60;

    // Simulation speed settings
    static constexpr double INITIAL_SIMULATION_SPEED = 1.0;
    static constexpr double MIN_SIMULATION_SPEED = 0.0625;
    static constexpr double MAX_SIMULATION_SPEED = 64.0;
    static constexpr double SIMULATION_SPEED_MULTIPLIER = 2.0;

    // Timing settings
    static constexpr double MAX_FRAME_TIME = 0.25;
};

} // namespace ftree::visualization
