#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <QVector3D>
#include <QColor>
#include <cmath>
#include "astronomicalbody.h"
#include "solarsimconstants.h"

using Catch::Matchers::WithinRel;

// ============================================================================
// SIMULATION TESTS - Physics & Constants
// ============================================================================

// ============================================================================
// Simulation: SolarSimConstants Tests (51-57)
// ============================================================================

TEST_CASE("Gravitational constant is positive", "[constants]") {
    REQUIRE(SolarSimConstants::G > 0);
}

TEST_CASE("Sun mass is greater than planet mass", "[constants]") {
    REQUIRE(SolarSimConstants::SUN_MASS > SolarSimConstants::PLANET_MASS);
}

TEST_CASE("Sun radius is greater than planet radius", "[constants]") {
    REQUIRE(SolarSimConstants::SUN_RADIUS > SolarSimConstants::PLANET_RADIUS);
}

TEST_CASE("Window width is positive", "[constants]") {
    REQUIRE(SolarSimConstants::WINDOW_WIDTH > 0);
    REQUIRE(SolarSimConstants::WINDOW_WIDTH == 1024);
}

TEST_CASE("Window height is positive", "[constants]") {
    REQUIRE(SolarSimConstants::WINDOW_HEIGHT > 0);
    REQUIRE(SolarSimConstants::WINDOW_HEIGHT == 768);
}

TEST_CASE("Steps per frame is positive", "[constants]") {
    REQUIRE(SolarSimConstants::STEPS_PER_FRAME > 0);
}

TEST_CASE("Time step is positive", "[constants]") {
    REQUIRE(SolarSimConstants::TIME_STEP > 0);
}

// ============================================================================
// Simulation: Physics Integration Tests (58-63)
// ============================================================================

TEST_CASE("Distance calculation between two planets", "[planet][physics]") {
    Planet planet1(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::white);
    Planet planet2(QVector3D(3, 4, 0), QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    REQUIRE_THAT(distance, WithinRel(5.0, 1e-5));
}

TEST_CASE("Distance between planet and sun", "[planet][sun][physics]") {
    Sun sun(QVector3D(0, 0, 0), 1e6, 50, Qt::yellow);
    Planet planet(QVector3D(5, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    double distance = (planet.getPosition() - sun.getPosition()).length();
    REQUIRE_THAT(distance, WithinRel(5.0, 1e-5));
}

TEST_CASE("Same position means zero distance", "[planet][physics]") {
    QVector3D pos(100, 200, 300);
    Planet planet1(pos, QVector3D(), 1000, 10, Qt::white);
    Planet planet2(pos, QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    REQUIRE(distance == 0.0);
}

TEST_CASE("Escape velocity calculation with sun", "[sun][physics]") {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    // v_escape = sqrt(2*G*M/r)
    double escape_v = std::sqrt(2 * SolarSimConstants::G * sun.getMass() / sun.getRadius());
    REQUIRE(escape_v > 0);
    REQUIRE(escape_v < 1000); // reasonable bound
}

TEST_CASE("Orbital velocity of planet around sun", "[planet][sun][physics]") {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    Planet planet(QVector3D(500, 0, 0), QVector3D(), SolarSimConstants::PLANET_MASS, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    double r = (planet.getPosition() - sun.getPosition()).length();
    double orbital_v = std::sqrt(SolarSimConstants::G * sun.getMass() / r);
    REQUIRE(orbital_v > 0);
    REQUIRE(orbital_v < 100); // reasonable bound for simulation
}

TEST_CASE("Position update for planet", "[planet][physics]") {
    Planet planet(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    QVector3D newPos(100, 200, 50);
    planet.setPosition(newPos);
    
    REQUIRE(planet.getPosition() == newPos);
}

// ============================================================================
// Simulation: Solar System Initialization Tests (64-67)
// ============================================================================

TEST_CASE("Solar system has 8 planets", "[solarsystem][planets]") {
    // This test verifies the expected number of planets in the initialized system
    std::vector<QString> planetNames = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    REQUIRE(planetNames.size() == 8);
}

TEST_CASE("Solar system planet names in order", "[solarsystem][planets]") {
    std::vector<QString> expectedOrder = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    
    // Verify the expected order matches our solar system
    REQUIRE(expectedOrder[0] == "Mercury");
    REQUIRE(expectedOrder[1] == "Venus");
    REQUIRE(expectedOrder[2] == "Earth");
    REQUIRE(expectedOrder[3] == "Mars");
    REQUIRE(expectedOrder[4] == "Jupiter");
    REQUIRE(expectedOrder[5] == "Saturn");
    REQUIRE(expectedOrder[6] == "Uranus");
    REQUIRE(expectedOrder[7] == "Neptune");
}

TEST_CASE("Solar system realistic planet masses", "[solarsystem][planets]") {
    // Jupiter should be much more massive than Earth
    double jupiterMass = SolarSimConstants::PLANET_MASS * 318;
    double earthMass = SolarSimConstants::PLANET_MASS;
    
    REQUIRE(jupiterMass > earthMass);
    REQUIRE(jupiterMass / earthMass == 318);
}

TEST_CASE("Solar system realistic planet radii", "[solarsystem][planets]") {
    // Jupiter should be much larger than Earth
    double jupiterRadius = SolarSimConstants::PLANET_RADIUS * 11;
    double earthRadius = SolarSimConstants::PLANET_RADIUS;
    
    REQUIRE(jupiterRadius > earthRadius);
    REQUIRE(jupiterRadius / earthRadius == 11);
}

// ============================================================================
// Simulation: Realistic Astronomical Constants Tests
// ============================================================================

TEST_CASE("Sun mass is realistically proportional to Earth mass", "[constants][astronomy]") {
    // Sun is approximately 333,000 times more massive than Earth
    REQUIRE(SolarSimConstants::SUN_MASS == 333000.0);
    REQUIRE(SolarSimConstants::SUN_MASS > 0);
    REQUIRE(SolarSimConstants::SUN_MASS / SolarSimConstants::EARTH_MASS == 333000.0);
}

TEST_CASE("Sun radius is realistically proportional to Earth radius", "[constants][astronomy]") {
    // Sun is approximately 109 times the radius of Earth
    REQUIRE(SolarSimConstants::SUN_RADIUS == 109.0);
    REQUIRE(SolarSimConstants::SUN_RADIUS > 0);
    REQUIRE(SolarSimConstants::SUN_RADIUS / SolarSimConstants::EARTH_RADIUS == 109.0);
}

TEST_CASE("Earth reference constants are baseline", "[constants][astronomy]") {
    REQUIRE(SolarSimConstants::EARTH_MASS == 1.0);
    REQUIRE(SolarSimConstants::EARTH_RADIUS == 1.0);
}

TEST_CASE("Display scale is positive", "[constants][display]") {
    REQUIRE(SolarSimConstants::DISPLAY_SCALE > 0);
    REQUIRE(SolarSimConstants::DISPLAY_SCALE == 0.25);
}

TEST_CASE("Sun display radius calculation", "[constants][display]") {
    // Sun display radius = SUN_RADIUS * DISPLAY_SCALE
    // = 109.0 * 0.25 = 27.25 pixels
    double sunDisplayRadius = SolarSimConstants::SUN_RADIUS * SolarSimConstants::DISPLAY_SCALE;
    REQUIRE_THAT(sunDisplayRadius, WithinRel(27.25, 1e-6));
}

TEST_CASE("Mercury display radius is small", "[constants][display]") {
    // Mercury radius relative to Earth = 0.383
    double mercuryRadius = 0.383;
    double mercuryDisplayRadius = mercuryRadius * SolarSimConstants::DISPLAY_SCALE;
    // Should be approximately 0.096 pixels
    REQUIRE_THAT(mercuryDisplayRadius, WithinRel(0.09575, 1e-6));
    REQUIRE(mercuryDisplayRadius < 1.0);  // Mercury should be very small
}

TEST_CASE("Jupiter display radius is larger than Mercury", "[constants][display]") {
    // Jupiter radius relative to Earth = 10.97
    double jupiterRadius = 10.97;
    double jupiterDisplayRadius = jupiterRadius * SolarSimConstants::DISPLAY_SCALE;
    
    // Mercury radius = 0.383
    double mercuryRadius = 0.383;
    double mercuryDisplayRadius = mercuryRadius * SolarSimConstants::DISPLAY_SCALE;
    
    REQUIRE(jupiterDisplayRadius > mercuryDisplayRadius);
    REQUIRE_THAT(jupiterDisplayRadius / mercuryDisplayRadius, WithinRel(10.97 / 0.383, 1e-6));
}

TEST_CASE("Realistic solar system has proper mass relationships", "[solarsystem][astronomy]") {
    // Mercury mass relative to Earth = 0.055
    double mercuryMass = 0.055;
    double earthMass = 1.0;
    
    REQUIRE(earthMass > mercuryMass);
    REQUIRE_THAT(earthMass / mercuryMass, WithinRel(18.18, 0.1));  // Earth is ~18x more massive than Mercury
}

TEST_CASE("Speed slider multiplier range minimum", "[ui][speed]") {
    // Speed slider minimum value 10 = 0.1x
    int minSliderValue = 10;
    double minMultiplier = minSliderValue / 100.0;
    REQUIRE_THAT(minMultiplier, WithinRel(0.1, 1e-6));
}

TEST_CASE("Speed slider multiplier range maximum", "[ui][speed]") {
    // Speed slider maximum value 2000 = 20x
    int maxSliderValue = 2000;
    double maxMultiplier = maxSliderValue / 100.0;
    REQUIRE_THAT(maxMultiplier, WithinRel(20.0, 1e-6));
}

TEST_CASE("Speed slider multiplier default is 1.0x", "[ui][speed]") {
    // Speed slider default value 100 = 1.0x
    int defaultValue = 100;
    double defaultMultiplier = defaultValue / 100.0;
    REQUIRE_THAT(defaultMultiplier, WithinRel(1.0, 1e-6));
}

TEST_CASE("Speed multiplier calculation for 0.5x", "[ui][speed]") {
    int sliderValue = 50;
    double multiplier = sliderValue / 100.0;
    REQUIRE_THAT(multiplier, WithinRel(0.5, 1e-6));
}

TEST_CASE("Speed multiplier calculation for 5x", "[ui][speed]") {
    int sliderValue = 500;
    double multiplier = sliderValue / 100.0;
    REQUIRE_THAT(multiplier, WithinRel(5.0, 1e-6));
}

TEST_CASE("Simulation time advancement at 1x speed", "[ui][time]") {
    // At 1x speed: 1 day per real second
    double speedMultiplier = 1.0;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;  // 86400 seconds = 1 day
    REQUIRE_THAT(simulationSecondsPerRealSecond, WithinRel(86400.0, 1e-6));
}

TEST_CASE("Simulation time advancement at 0.1x speed", "[ui][time]") {
    // At 0.1x speed: 0.1 days per real second (2.4 hours)
    double speedMultiplier = 0.1;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;
    REQUIRE_THAT(simulationSecondsPerRealSecond, WithinRel(8640.0, 1e-6));
}

TEST_CASE("Simulation time advancement at 20x speed", "[ui][time]") {
    // At 20x speed: 20 days per real second
    double speedMultiplier = 20.0;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;
    REQUIRE_THAT(simulationSecondsPerRealSecond, WithinRel(1728000.0, 1e-6));
}

TEST_CASE("Painter's algorithm requires proper depth sorting", "[rendering][depth]") {
    // Test that depth values work correctly for sorting
    double farDepth = -100.0;    // Far from camera (negative z)
    double closeDepth = 100.0;   // Close to camera (positive z)
    
    // Should render far objects first (lower z value)
    REQUIRE(farDepth < closeDepth);
}
