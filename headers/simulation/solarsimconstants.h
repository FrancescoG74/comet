#pragma once

// Physical and simulation constants
namespace SolarSimConstants {
    // Gravitational constant (simulated for computational purposes)
    constexpr double G = 0.0012;
    
    // Sun's astronomical properties
    // Real values: Mass ≈ 1.989 × 10^30 kg, Radius ≈ 696,000 km, Volume ≈ 1.41 × 10^27 km³
    // Density ≈ 1,409 kg/m³
    constexpr double SUN_MASS = 333000.0;  // Relative to Earth mass (1.0)
    constexpr double SUN_RADIUS = 109.0;   // Relative to Earth radius (1.0)
    
    // Reference masses and radii (in relative units)
    // Earth mass = 1.0, Earth radius = 1.0
    constexpr double EARTH_MASS = 1.0;
    constexpr double EARTH_RADIUS = 1.0;
    
    // Generic planet scaling for physics calculations (used for unknown bodies)
    constexpr double PLANET_MASS = 1.0;     // Relative to Earth
    constexpr double PLANET_RADIUS = 1.0;   // Relative to Earth
    
    // Window and display settings
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 768;
    constexpr int MARGIN = 100;
    
    // Display/Visualization scaling
    // Global scale factor to convert astronomical radius values to screen pixels
    // Adjust this to zoom in/out on planetary sizes
    constexpr double DISPLAY_SCALE = 0.25;  // Pixels per Earth radius unit
    
    // Simulation parameters
    constexpr int STEPS_PER_FRAME = 5;      // Number of physics steps per frame
    constexpr double TIME_STEP = 1.0;       // Time step for each physics calculation
    constexpr int TIMER_INTERVAL_MS = 16;   // Timer interval in milliseconds (~60 FPS)
}
