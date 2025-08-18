#pragma once

// Physical and simulation constants
namespace SolarSimConstants {
    constexpr double G = 0.0012; // Simulated gravitational constant
    constexpr double SUN_MASS = 1000000; // Mass of the sun
    constexpr double SUN_RADIUS = 40;
    constexpr double PLANET_MASS = 1000;
    constexpr double PLANET_RADIUS = 20;
    constexpr int WINDOW_WIDTH = 1200;
    constexpr int WINDOW_HEIGHT = 800;
    constexpr int MARGIN = 100;
    constexpr int STEPS_PER_FRAME = 5;
    constexpr double TIME_STEP = 1.0;
    constexpr int TIMER_INTERVAL_MS = 16;
}
